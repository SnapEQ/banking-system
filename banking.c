#include "banking.h"
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void initBankingSystem()
{
    return;
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

            buffSize *= 2;
            char *tmp = realloc(buffer, buffSize);
            if (tmp == NULL)
            {
                free(buffer);
                return NULL;
            }

            buffer = tmp;
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

    if (input == NULL || *length > 11)
    {
        return NULL;
    }

    return input;
}

int findUser(const char *s, searchType type)
{
    FILE *fptr;

    fptr = fopen("db.txt", "r");
    if (fptr == NULL || s == NULL) {
        return -2;
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int lineNum = 0;
    int maxSemiNo = 0;

    switch (type){
        case ID:
            maxSemiNo = 1;
            break;
        case NAME:
            maxSemiNo = 2;
            break;
        case SURNAME:
            maxSemiNo = 3;
            break;
        case ADDRESS:
            maxSemiNo = 4;
            break;
        case PESEL:
            maxSemiNo = 5;
            break;
        default:
            printf("Wrong type of search");
            break;
    }

    while ((read = getline(&line, &len, fptr)) != -1) {
        size_t index = 0;
        int howManySemi = 0;
        size_t buffSize = 256;
        size_t buffIndex = 0;
        char *buff = malloc(buffSize * sizeof(char));
        while(howManySemi < maxSemiNo) {

            if (howManySemi == maxSemiNo - 1 && line[index] != ';')
            {

                buff[buffIndex] = line[index];

                if (buffIndex + 1 > buffSize) {
                    buffSize *= 2;
                    char *tmp = realloc(buff, buffSize);
                    if (tmp == NULL) {
                       free(buff);
                        return -2;
                    }

                    buff = tmp;
                }
                buffIndex++;

            }

            if (line[index] == ';') {
                howManySemi++;
            }
            index++;
        }
        buff[buffIndex] = '\0';

        if (strcmp(s, buff) == 0) {
            free(buff);
            free(line);
            fclose(fptr);
            return lineNum;
        }

        lineNum++;
    }

    if (line) {
        free(line);
    }

    fclose(fptr);
    return -1;
}

char *generateId()
{
    static const char alpha[] = "1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    const int maxAttempts = 10000;


    char *id = malloc(5);
    if (id == NULL) {
        return NULL;
    }

    for (int attempt = 0; attempt < maxAttempts; attempt++) {
        for (int i = 0; i < 4; i++) {
            id[i] = alpha[rand() % (sizeof(alpha) - 1)];
        }
        id[4] = '\0';

        int r = findUser(id, ID);
        if (r == -1) {
            return id;
        }
        if (r == -2) {
            free(id);
            return NULL;
        }
    }

    free(id);
    return NULL;
}

void storeNewUser()
{
    FILE *fptr;

    fptr = fopen("db.txt", "a+");
    if (fptr == NULL)
    {
        return;
    }

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

    int peselLine = findUser(userPesel, PESEL); 
    if (peselLine >= 0)
    {
        printf("There is a person with such pesel existing in the database! \nThe user was not registered");
        return;
    }

    printf("%s", userId);
    fprintf(fptr, "%s;%s;%s;%s;%s;%s;%s\n", userId, userName, userSurname, userAddress, userPesel, "0", "0");
 
    free(userId);
    free(userName);
    free(userSurname);
    free(userAddress);
    free(userPesel);
    fclose(fptr);
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

    while (token != NULL && i < 7)
    {
        fields[i++] = token;
        token = strtok(NULL, ";");
    }

    return i;
}

int calculateNextBalance(const char *balanceField, long delta, bool checkFunds, long *nextBalance)
{
    char *endptr = NULL;
    long currentBalance = strtol(balanceField, &endptr, 10);
    if (endptr == balanceField)
    {
        return -2;
    }

    *nextBalance = currentBalance + delta;
    if (checkFunds && *nextBalance < 0)
    {
        return -3;
    }

    return 0;
}

void writeUserLineWithNewBalance(FILE *dst, char **fields, long nextBalance)
{
    trimNewline(fields[6]);
    fprintf(dst, "%s;%s;%s;%s;%s;%ld;%s\n",
            fields[0],
            fields[1],
            fields[2],
            fields[3],
            fields[4],
            nextBalance,
            fields[6]);
}

int processBalanceLine(FILE *dst, const char *line, const char *userId, long delta, bool checkFunds, bool *updated)
{
    char *copy = strdup(line);
    if (copy == NULL)
    {
        return -2;
    }

    char *fields[7] = {0};
    int fieldCount = splitUserLine(copy, fields);

    if (fieldCount == 7 && strcmp(fields[0], userId) == 0)
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
    if (userId == NULL)
    {
        return -2;
    }

    FILE *src = fopen("db.txt", "r");
    FILE *dst = fopen("db.tmp", "w");

    if (src == NULL || dst == NULL)
    {
        if (src != NULL)
        {
            fclose(src);
        }
        if (dst != NULL)
        {
            fclose(dst);
        }
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

    remove("db.txt");
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

int withdrawl(const char *userId, long amount)
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

    int w = withdrawl(fromUserId, amount);
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

void listAllUsers() {
    FILE *fptr;
    fptr = fopen("db.txt", "r");

    if (fptr == NULL)
    {
        return;
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, fptr)) != -1)
    {
        printf("%s", line);
        fflush(stdout);
    }


    fclose(fptr);
}
