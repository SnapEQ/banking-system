#include "banking.h"
#include <stdint.h>
#include <errno.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>

//  TODO
//  Display error when no id is found during operations
//  Validate pesel upon registration

const char ban[] = " _______                       __        __                     \n|       \\                     |  \\      |  \\                    \n| $$$$$$$\\  ______   _______  | $$   __  \\$$ _______    ______  \n| $$__/ $$ |      \\ |       \\ | $$  /  \\|  \\|       \\  /      \\ \n| $$    $$  \\$$$$$$\\| $$$$$$$\\| $$_/  $$| $$| $$$$$$$\\|  $$$$$$\\\n| $$$$$$$\\ /      $$| $$  | $$| $$   $$ | $$| $$  | $$| $$  | $$\n| $$__/ $$|  $$$$$$$| $$  | $$| $$$$$$\\ | $$| $$  | $$| $$__| $$\n| $$    $$ \\$$    $$| $$  | $$| $$  \\$$\\| $$| $$  | $$ \\$$    $$\n \\$$$$$$$   \\$$$$$$$ \\$$   \\$$ \\$$   \\$$ \\$$ \\$$   \\$$ _\\$$$$$$$\n                                                      |  \\__| $$\n                                                       \\$$    $$\n                                                        \\$$$$$$ \n  ______                         __                             \n /      \\                       |  \\                            \n|  $$$$$$\\ __    __   _______  _| $$_     ______   ______ ____  \n| $$___\\$$|  \\  |  \\ /       \\|   $$ \\   /      \\ |      \\    \\ \n \\$$    \\ | $$  | $$|  $$$$$$$ \\$$$$$$  |  $$$$$$\\| $$$$$$\\$$$$\\\n _\\$$$$$$\\| $$  | $$ \\$$    \\   | $$ __ | $$    $$| $$ | $$ | $$\n|  \\__| $$| $$__/ $$ _\\$$$$$$\\  | $$|  \\| $$$$$$$$| $$ | $$ | $$\n \\$$    $$ \\$$    $$|       $$   \\$$  $$ \\$$     \\| $$ | $$ | $$\n  \\$$$$$$  _\\$$$$$$$ \\$$$$$$$     \\$$$$   \\$$$$$$$ \\$$  \\$$  \\$$\n          |  \\__| $$                                            \n           \\$$    $$                                            \n            \\$$$$$$                                             \n";

void initBankingSystem()
{
    printMenu();
}

char *getUserInput(size_t *length)
{
    size_t buffSize = 1024;
    *length = 0;

    char *buffer = (char *)malloc(sizeof(char) * buffSize);

    if (buffer == NULL)
    {
        return NULL;
    }

    int ch;

    while ((ch = getchar()) != '\n' && ch != EOF)
    {

        if (*length + 1 >= buffSize)
        {
            if (buffSize > SIZE_MAX / 2)
            {
                free(buffer);
                return NULL;
            }

            buffSize *= 2;
            char *tmp = realloc(buffer, buffSize);
            if (tmp == NULL)
            {
                free(buffer);
                return NULL;
            }

            buffer = tmp;
        }

        if ((char)ch == ';')
        {
            continue;
        }

        buffer[(*length)++] = (char)ch;
    }

    if (ch == EOF && *length == 0)
    {
        free(buffer);
        return NULL;
    }

    buffer[*length] = '\0';

    return buffer;
}

char *getUserName(size_t *length)
{
    printf("Provide your name: ");
    fflush(stdout);
    char *input = getUserInput(length);

    if (input == NULL)
    {
        return NULL;
    }

    return input;
}

char *getUserSurname(size_t *length)
{
    printf("Provide your surname: ");
    fflush(stdout);
    char *input = getUserInput(length);

    if (input == NULL)
    {
        return NULL;
    }

    return input;
}

char *getUserAddress(size_t *length)
{
    printf("The address should be in form of this: \n 'Kurowa 123 Wadowice 12-345' \n");
    printf("Provide your address: ");
    fflush(stdout);

    char *input = getUserInput(length);

    if (input == NULL)
    {
        return NULL;
    }

    return input;
}

char *getUserPesel(size_t *length)
{
    printf("Provide you PESEL number: ");
    fflush(stdout);
    char *input = getUserInput(length);

    if (input == NULL)
    {
        return NULL;
    }

    return input;
}

char *getRegistrationNo(size_t *length)
{
    printf("Provide your registration number (7-16 letters/digits):  ");
    fflush(stdout);
    char *input = getUserInput(length);

    if (input == NULL)
    {
        return NULL;
    }

    if (*length < 7 || *length > 16)
    {
        free(input);
        return NULL;
    }

    for (size_t i = 0; i < *length; i++)
    {
        if (!isalnum((unsigned char)input[i]))
        {
            free(input);
            return NULL;
        }
    }

    if (strlen(input) != *length)
    {
        free(input);
        return NULL;
    }

    return input;
}

int semicolonLimitForSearchType(searchType type)
{
    switch (type)
    {
    case ID:
        return 1;
    case NAME:
        return 2;
    case SURNAME:
        return 3;
    case ADDRESS:
        return 4;
    case PESEL:
        return 5;
    default:
        return -1;
    }
}

int appendToDynamicBuffer(char **buffer, size_t *bufferSize, size_t *bufferIndex, char c)
{
    if (*bufferIndex + 1 >= *bufferSize)
    {
        if (*bufferSize > ((size_t)-1) / 2)
        {
            return -1;
        }

        *bufferSize *= 2;
        char *tmp = realloc(*buffer, *bufferSize);
        if (tmp == NULL)
        {
            return -1;
        }

        *buffer = tmp;
    }

    (*buffer)[(*bufferIndex)++] = c;
    return 0;
}

int extractSearchField(const char *line, int semicolonLimit, char **outField)
{
    size_t index = 0;
    int semicolonCount = 0;
    size_t bufferSize = 256;
    size_t bufferIndex = 0;
    char *buffer = malloc(bufferSize * sizeof(char));

    if (buffer == NULL)
    {
        return -2;
    }

    while (semicolonCount < semicolonLimit && line[index] != '\0')
    {
        if (semicolonCount == semicolonLimit - 1 && line[index] != ';')
        {
            if (appendToDynamicBuffer(&buffer, &bufferSize, &bufferIndex, line[index]) != 0)
            {
                free(buffer);
                return -2;
            }
        }

        if (line[index] == ';') semicolonCount++;
        index++;
    }

    if (semicolonCount < semicolonLimit)
    {
        free(buffer);
        return 0;
    }

    buffer[bufferIndex] = '\0';
    *outField = buffer;
    return 1;
}

int doesLineMatchSearch(const char *line, const char *value, int semicolonLimit)
{
    char *field = NULL;
    int extractionResult = extractSearchField(line, semicolonLimit, &field);

    if (extractionResult <= 0)
    {
        return extractionResult;
    }

    int isMatch = strcmp(value, field) == 0;
    free(field);
    return isMatch ? 1 : 0;
}

int findUser(const char *s, searchType type)
{
    if (s == NULL) return -2;

    int semicolonLimit = semicolonLimitForSearchType(type);
    if (semicolonLimit < 0) return -2;

    FILE *fptr = fopen("db.txt", "r");
    if (fptr == NULL) return -2;

    char *line = NULL;
    size_t len = 0;
    int lineNum = 0;

    while (getline(&line, &len, fptr) != -1)
    {
        int lineMatchResult = doesLineMatchSearch(line, s, semicolonLimit);
        if (lineMatchResult == -2)
        {
            free(line);
            fclose(fptr);
            return -2;
        }

        if (lineMatchResult == 1)
        {
            free(line);
            fclose(fptr);
            return lineNum;
        }

        lineNum++;
    }

    free(line);
    fclose(fptr);
    return -1;
}

char *generateId()
{
    static const char alpha[] = "1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    const int maxAttempts = 10000;

    char *id = malloc(5);
    if (id == NULL) return NULL;

    for (int attempt = 0; attempt < maxAttempts; attempt++)
    {
        for (int i = 0; i < 4; i++)
        {
            id[i] = alpha[rand() % (sizeof(alpha) - 1)];
        }
        id[4] = '\0';

        int r = findUser(id, ID);
        if (r == -1) return id;
    
        if (r == -2)
        {
            free(id);
            return NULL;
        }
    }

    free(id);
    return NULL;
}

void trimNewline(char *s)
{
    if (s == NULL)
    {
        return;
    }

    size_t l = strlen(s);
    if (l > 0 && s[l - 1] == '\n')
    {
        s[l - 1] = '\0';
    }
}

int splitUserLine(char *line, char **fields)
{
    int i = 0;
    char *token = strtok(line, ";");

    while (token != NULL && i < 6)
    {
        fields[i++] = token;
        token = strtok(NULL, ";");
    }

    return i;
}

int calculateNextBalance(const char *balanceField, long delta, bool checkFunds, long *nextBalance)
{
    if (balanceField == NULL || nextBalance == NULL) return -2;

    if (balanceField[0] == '\0') return -2;

    for (size_t i = 0; balanceField[i] != '\0'; i++)
    {
        if (!isdigit((unsigned char)balanceField[i])) return -2;
    }

    char *endptr = NULL;
    errno = 0;
    long currentBalance = strtol(balanceField, &endptr, 10);
    if (endptr == balanceField || errno != 0 || *endptr != '\0') return -2;
    

    if (delta > 0 && currentBalance > LONG_MAX - delta) return -2;

    if (delta < 0 && currentBalance < LONG_MIN - delta) return -2;

    *nextBalance = currentBalance + delta;
    if (checkFunds && *nextBalance < 0) return -3;

    return 0;
}

void writeUserLineWithNewBalance(FILE *dst, char **fields, long nextBalance)
{
    fprintf(dst, "%s;%s;%s;%s;%s;%ld\n",
            fields[0],
            fields[1],
            fields[2],
            fields[3],
            fields[4],
            nextBalance);
}

int processBalanceLine(FILE *dst, const char *line, const char *userId, long delta, bool checkFunds, bool *updated)
{
    char *copy = strdup(line);
    if (copy == NULL)
    {
        return -2;
    }

    char *fields[6] = {0};
    int fieldCount = splitUserLine(copy, fields);

    if (fieldCount >= 6 && strcmp(fields[0], userId) == 0)
    {
        long nextBalance = 0;
        int calcResult = calculateNextBalance(fields[5], delta, checkFunds, &nextBalance);
        if (calcResult != 0)
        {
            free(copy);
            return calcResult;
        }

        writeUserLineWithNewBalance(dst, fields, nextBalance);
        *updated = true;
    }
    else
    {
        fputs(line, dst);
    }

    free(copy);
    return 0;
}

int updateUserBalance(const char *userId, long delta, bool checkFunds)
{
    if (userId == NULL) return -2;

    FILE *src = fopen("db.txt", "r");
    FILE *dst = fopen("db.tmp", "w");

    if (src == NULL || dst == NULL)
    {
        if (src != NULL) fclose(src);
        if (dst != NULL) fclose(dst);
        return -2;
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    bool found = false;

    while ((read = getline(&line, &len, src)) != -1)
    {
        int lineResult = processBalanceLine(dst, line, userId, delta, checkFunds, &found);
        if (lineResult != 0)
        {
            free(line);
            fclose(src);
            fclose(dst);
            remove("db.tmp");
            return lineResult;
        }
    }

    free(line);
    fclose(src);
    fclose(dst);

    if (!found)
    {
        remove("db.tmp");
        return -1;
    }

    if (rename("db.tmp", "db.txt") != 0)
    {
        remove("db.tmp");
        return -2;
    }

    return 0;
}

int makeDeposit(const char *userId, long amount)
{
    if (amount <= 0)
    {
        return -2;
    }

    return updateUserBalance(userId, amount, false);
}

int withdrawal(const char *userId, long amount)
{
    if (amount <= 0)
    {
        return -2;
    }

    return updateUserBalance(userId, -amount, true);
}

int transfer(const char *fromUserId, const char *toUserId, long amount)
{
    if (amount <= 0 || fromUserId == NULL || toUserId == NULL)
    {
        return -2;
    }

    if (strcmp(fromUserId, toUserId) == 0)
    {
        return 0;
    }

    int w = withdrawal(fromUserId, amount);
    if (w != 0)
    {
        return w;
    }

    int d = makeDeposit(toUserId, amount);
    if (d != 0)
    {
        int rollback = makeDeposit(fromUserId, amount);
        if (rollback != 0)
        {
            return -2;
        }
        return d;
    }

    return 0;
}

int makeOperation(operationType operationType, const char *firstUserId, const char *secondUserId, long amount)
{
    if (firstUserId == NULL)
        return -1;
    if (operationType == TRANSFER && secondUserId == NULL)
        return -1;

    int res = 0;

    switch (operationType)
    {
    case DEPOSIT:
        res = makeDeposit(firstUserId, amount);
        break;
    case WITHDRAWAL:
        res = withdrawal(firstUserId, amount);
        break;
    case TRANSFER:
        res = transfer(firstUserId, secondUserId, amount);
        break;
    }

    return res;
}

int takeOutInsurance(const char *userId, const char *registrationNo, long amount)
{
    if (!userId || !registrationNo || amount <= 0) return -2;

    if (findUser(userId, ID) < 0) return -1;

    FILE *fptr = fopen("ins.txt", "a+");
    if (fptr == NULL) return -2;


    rewind(fptr);
    char *line = NULL;
    size_t len = 0;

    while (getline(&line, &len, fptr) != -1)
    {
        char *copy = strdup(line);
        if (copy == NULL)
        {
            free(line);
            fclose(fptr);
            return -2;
        }

        char *token = strtok(copy, ";");
        token = strtok(NULL, ";");

        if (token != NULL && strcmp(token, registrationNo) == 0)
        {
            free(copy);
            free(line);
            fclose(fptr);
            return -3;
        }

        free(copy);
    }

    free(line);

    if (fprintf(fptr, "%s;%s;%ld\n", userId, registrationNo, amount) < 0)
    {
        fclose(fptr);
        return -2;
    }

    fclose(fptr);
    return 0;
}

void printUserInsurances(const char *userId)
{
    printf("+----------------------------------------+\n");
    printf("| Insurances                             |\n");
    printf("+----------------------------------------+\n");

    if (userId == NULL)
    {
        printf("| %-38s |\n", "none");
        printf("+----------------------------------------+\n");
        return;
    }

    FILE *fptr = fopen("ins.txt", "r");
    if (fptr == NULL)
    {
        printf("| %-38s |\n", "none");
        printf("+----------------------------------------+\n");
        return;
    }

    char *line = NULL;
    size_t len = 0;
    bool hasInsurance = false;

    while (getline(&line, &len, fptr) != -1)
    {
        char *copy = strdup(line);
        if (copy == NULL) continue;

        char *insuranceUserId = strtok(copy, ";");
        char *registrationNo = strtok(NULL, ";");
        char *amount = strtok(NULL, ";");

        if (insuranceUserId != NULL && registrationNo != NULL && amount != NULL && strcmp(insuranceUserId, userId) == 0)
        {
            trimNewline(amount);
            printf("| Registration: %-24s |\n", registrationNo);
            printf("| Yearly: %-30s |\n", amount);
            printf("+----------------------------------------+\n");
            hasInsurance = true;
        }

        free(copy);
    }

    if (!hasInsurance)
    {
        printf("| %-38s |\n", "none");
        printf("+----------------------------------------+\n");
    }

    free(line);
    fclose(fptr);
}

void printUserFormatted(const char *line)
{
    if (line == NULL) return;

    char *copy = strdup(line);
    if (copy == NULL) return;

    char *fields[6] = {0};
    int fieldCount = splitUserLine(copy, fields);
    if (fieldCount < 6)
    {
        printf("%s", line);
        free(copy);
        return;
    }

    trimNewline(fields[5]);
    printf("+----------------------------------------+\n");
    printf("| ID      : %-28s |\n", fields[0]);
    printf("| Name    : %-28s |\n", fields[1]);
    printf("| Surname : %-28s |\n", fields[2]);
    printf("| Address : %-28s |\n", fields[3]);
    printf("| PESEL   : %-28s |\n", fields[4]);
    printf("| Balance : %-28s |\n", fields[5]);
    printf("+----------------------------------------+\n");
    printUserInsurances(fields[0]);
    printf("\n");

    free(copy);
}

void listUser(int lineSearchNum)
{
    FILE *fptr;
    fptr = fopen("db.txt", "r");

    if (fptr == NULL) return;

    char *line = NULL;
    size_t len = 0;

    int lineIdx = 0;

    while (getline(&line, &len, fptr) != -1)
    {
        if (lineIdx == lineSearchNum)
        {
            printUserFormatted(line);
            free(line);
            fclose(fptr);
            return;
        }
        lineIdx++;
    }
    printf("Incorrect line index \n");
    free(line);
    fclose(fptr);
}

void listAllUsers()
{
    FILE *fptr;
    fptr = fopen("db.txt", "r");

    if (fptr == NULL) return;

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, fptr)) != -1)
    {
        printUserFormatted(line);
    }

    free(line);
    fclose(fptr);
}

void clearScreen()
{
    printf("\033[2J\033[H");
    fflush(stdout);
}

void waitForEnter()
{
    int ch;
    printf("\nPress Enter to continue...");
    fflush(stdout);
    while ((ch = getchar()) != '\n' && ch != EOF)
    {
    }
}

int readIntInRange(const char *prompt, int min, int max)
{
    char buf[32];
    char *end;
    long v;

    while (true)
    {
        printf("%s", prompt);
        if (!fgets(buf, sizeof(buf), stdin))
            return min;

        errno = 0;
        v = strtol(buf, &end, 10);

        if (errno == 0 && end != buf && (*end == '\n' || *end == '\0') && v >= min && v <= max)
        {
            return (int)v;
        }

        printf("Invalid choice. Enter %d-%d.\n", min, max);
    }
}

long readLongInRange(const char *prompt, long min, long max)
{
    char buf[64];
    char *end;
    long v;

    while (true)
    {
        printf("%s", prompt);
        if (!fgets(buf, sizeof(buf), stdin))
            return 0;

        errno = 0;
        v = strtol(buf, &end, 10);

        if (errno == 0 && end != buf && (*end == '\n' || *end == '\0') && v >= min && v <= max)
        {
            return v;
        }

        printf("Invalid choice, Enter %ld-%ld.\n", min, max);
    }
}

char readConfirmation(const char *prompt, char def)
{
    char buf[8];
    def = (char)tolower((unsigned char)def);

    while (true)
    {
        printf("%s", prompt);
        fflush(stdout);

        if (!fgets(buf, sizeof(buf), stdin))
            return def;
        if (buf[0] == '\n')
            return def;

        char c = (char)tolower((unsigned char)buf[0]);

        if ((c == 'y' || c == 'n') && (buf[1] == '\n' || buf[1] == '\0'))
        {
            return c;
        }

        printf("Type Y or n.\n");
    }
}

bool validateNonEmpty(const char *s)
{
    return s != NULL && s[0] != '\0';
}

bool validateId(const char *s)
{
    if (!s || strlen(s) != 4)
        return false;
    for (int i = 0; i < 4; i++)
    {
        if (!isalnum((unsigned char)s[i]))
            return false;
    }
    return true;
}

bool validatePesel(const char *s)
{
    if (!s || strlen(s) != 11)
        return false;
    for (int i = 0; i < 11; i++)
    {
        if (!isdigit((unsigned char)s[i]))
            return false;
    }
    return true;
}

const SearchOption SEARCH_OPTIONS[] = {
    {ID, "ID", validateId},
    {NAME, "Name", validateNonEmpty},
    {SURNAME, "Surname", validateNonEmpty},
    {ADDRESS, "Address", validateNonEmpty},
    {PESEL, "PESEL", validatePesel},
};

int searchOptionsCount()
{
    return (int)(sizeof(SEARCH_OPTIONS) / sizeof(SEARCH_OPTIONS[0]));
}

const OperationOption OPERATION_OPTIONS[] = {
    {DEPOSIT, "Deposit", validateId},
    {WITHDRAWAL, "Withdrawal", validateId},
    {TRANSFER, "Transfer", validateId},
};

int operationOptionsCount()
{
    return (int)(sizeof(OPERATION_OPTIONS) / sizeof(OPERATION_OPTIONS[0]));
}

char *readValidatedValue(const SearchOption *opt)
{
    while (1)
    {
        size_t len = 0;
        printf("Enter %s (0 to go back): ", opt->label);
        fflush(stdout);

        char *value = getUserInput(&len);
        if (!value)
            return NULL;
        if (strcmp(value, "0") == 0)
        {
            free(value);
            return NULL;
        }

        if (opt->validate(value))
            return value;

        printf("Invalid %s format.\n", opt->label);
        free(value);
    }
}

char *readValidatedValueOperation(const OperationOption *opt)
{
    while (1)
    {
        size_t len = 0;
        printf("Enter Id (0 to go back): ");
        fflush(stdout);

        char *value = getUserInput(&len);
        if (!value)
            return NULL;
        if (strcmp(value, "0") == 0)
        {
            free(value);
            return NULL;
        }

        if (opt->validate(value))
            return value;

        printf("Invalid ID format.\n");
        free(value);
    }
}

char *readValidatedID()
{
    while (1)
    {
        size_t len = 0;
        printf("Enter Id (0 to go back): ");
        fflush(stdout);

        char *value = getUserInput(&len);
        if (!value)
            return NULL;
        if (strcmp(value, "0") == 0)
        {
            free(value);
            return NULL;
        }

        if (validateId(value))
            return value;

        printf("Invalid ID format.\n");
        free(value);
    }
}

void insuranceSubmenu()
{
    while (true)
    {
        clearScreen();
        printf("=== Insurances ===\n");
        printf("1. Take out insurance\n");
        printf("0. Back\n\n");

        int choice = readIntInRange("Choose: ", 0, 1);

        if (choice == 0) return;

        if (choice == 1)
        {
            char *userId = readValidatedID();
            if (!userId)
            {
                continue;
            }

            size_t length = 0;
            char *registrationNo = NULL;
            registrationNo = getRegistrationNo(&length);
            long amount = readLongInRange("Type in the amount of the yearly payment: ", 1, LONG_MAX);

            int res = takeOutInsurance(userId, registrationNo, amount);

            if (!registrationNo || res < 0)
            {
                if (!registrationNo)
                {
                    printf("Invalid registration number format.\n");
                }
                else if (res == -1)
                {
                    printf("User ID not found in database.\n");
                }
                else if (res == -3)
                {
                    printf("Insurance already exists for this registration number.\n");
                }
                else
                {
                    printf("Failed to take out insurance. Please try again.\n");
                }

                free(userId);
                free(registrationNo);
                waitForEnter();
                continue;
            }

            printf("Insurance has been registered for user: %s\n", userId);
            free(userId);
            free(registrationNo);
            waitForEnter();
        }
    }
}

void listUserSubmenu()
{
    while (true)
    {
        clearScreen();
        printf("=== List Users By ===\n");
        for (int i = 0; i < searchOptionsCount(); i++)
        {
            printf("%d. %s\n", i + 1, SEARCH_OPTIONS[i].label);
        }
        printf("%d. All Users\n", searchOptionsCount() + 1);
        printf("0. Back\n\n");

        int choice = readIntInRange("Choose: ", 0, searchOptionsCount() + 1);

        if (choice == 0)
            return;

        if (choice == searchOptionsCount() + 1)
        {
            listAllUsers();
            waitForEnter();
            continue;
        }

        const SearchOption *opt = &SEARCH_OPTIONS[choice - 1];
        char *value = readValidatedValue(opt);
        if (!value)
            continue;

        int line = findUser(value, opt->type);
        if (line >= 0)
            listUser(line);
        else
            printf("User not found\n");

        free(value);
        waitForEnter();
    }
}

void operationsSubmenu()
{
    while (true)
    {
        clearScreen();
        printf("=== Operations ===\n");
        for (int i = 0; i < operationOptionsCount(); i++)
        {
            printf("%d. %s\n", i + 1, OPERATION_OPTIONS[i].label);
        }
        printf("0. Back\n\n");

        int choice = readIntInRange("Choose: ", 0, operationOptionsCount());

        if (choice == 0)
            return;

        const OperationOption *opt = &OPERATION_OPTIONS[choice - 1];

        char *userIdValidated = NULL;
        char *secondUserIdValidated = NULL;

        userIdValidated = readValidatedValueOperation(opt);
        if (!userIdValidated)
            continue;

        if (opt->type == TRANSFER)
        {
            secondUserIdValidated = readValidatedValueOperation(opt);
            if (!secondUserIdValidated)
            {
                printf("Second user accountNumber is required for transfer.\n");
                free(userIdValidated);
                waitForEnter();
                continue;
            }
        }

        long amount = readLongInRange("Enter amount: ", 1, LONG_MAX);

        if (amount == 0l)
        {
            printf("Line parsing went wrong, Try again\n");
        }
        else if (readConfirmation("Do you want to proceed? (Y/n): ", 'n') == 'y')
        {
            int res = makeOperation(opt->type, userIdValidated, secondUserIdValidated, amount);
            if (res < 0)
            {
                printf("Operation failed\n");
                free(userIdValidated);
                free(secondUserIdValidated);
                waitForEnter();
                continue;
            }
        }

        free(secondUserIdValidated);
        free(userIdValidated);
        waitForEnter();
    }
}

void storeNewUser()
{
    FILE *fptr;

    fptr = fopen("db.txt", "a+");
    if (fptr == NULL) return;

    clearScreen();

    size_t length = 0;
    char *userId = generateId();
    char *userName = getUserName(&length);
    char *userSurname = getUserSurname(&length);
    char *userAddress = getUserAddress(&length);

    char *userPesel = getUserPesel(&length);

    if (userId == NULL || userName == NULL || userSurname == NULL || userAddress == NULL || userPesel == NULL)
    {
        free(userId);
        free(userName);
        free(userSurname);
        free(userAddress);
        free(userPesel);
        fclose(fptr);
        return;
    }

    if (!validatePesel(userPesel))
    {
        printf("Wrong pesel format");
        free(userId);
        free(userName);
        free(userSurname);
        free(userAddress);
        free(userPesel);
        fclose(fptr);
        return;
    }

    int peselLine = findUser(userPesel, PESEL);
    if (peselLine == -2)
    {
        printf("Could not validate PESEL due to a database error.\n");
        free(userId);
        free(userName);
        free(userSurname);
        free(userAddress);
        free(userPesel);
        fclose(fptr);
        return;
    }

    if (peselLine >= 0)
    {
        printf("There is a person with such pesel existing in the database! \nThe user was not registered");
        free(userId);
        free(userName);
        free(userSurname);
        free(userAddress);
        free(userPesel);
        fclose(fptr);

        return;
    }

    printf("Here is your bank account number: %s", userId);
    fprintf(fptr, "%s;%s;%s;%s;%s;%s\n", userId, userName, userSurname, userAddress, userPesel, "0");

    free(userId);
    free(userName);
    free(userSurname);
    free(userAddress);
    free(userPesel);
    fclose(fptr);
}

void printMenu()
{
    while (true)
    {
        clearScreen();
        printf("%s\n", ban);
        printf("List of operations\n");
        printf("1 List user\n");
        printf("2 Operations\n");
        printf("3 Register account\n");
        printf("4 Insurances\n");
        printf("0 Exit\n\n");

        int choice = readIntInRange("Choose: ", 0, 4);

        switch (choice)
        {
        case 1:
            listUserSubmenu();
            break;
        case 2:
            operationsSubmenu();
            break;
        case 3:
            storeNewUser();
            waitForEnter();
            break;
        case 4:
            insuranceSubmenu();
            break;
        case 0:
            return;
        default:
            break;
        }
    }
}
