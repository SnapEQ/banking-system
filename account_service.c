#include "account_service.h"

#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "input.h"
#include "ui_render.h"
#include "user_repository.h"
#include "validation.h"

static char *getUserName(size_t *length)
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

static char *getUserSurname(size_t *length)
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

static char *getUserAddress(size_t *length)
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

static char *getUserPesel(size_t *length)
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

static char *generateId(void)
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
        if (r == RESULT_NOT_FOUND) return id;

        if (r == RESULT_ERROR)
        {
            free(id);
            return NULL;
        }
    }

    free(id);
    return NULL;
}

int makeDeposit(const char *userId, long amount)
{
    if (amount <= 0)
    {
        return RESULT_ERROR;
    }

    return updateUserBalance(userId, amount, false);
}

int withdrawal(const char *userId, long amount)
{
    if (amount <= 0)
    {
        return RESULT_ERROR;
    }

    return updateUserBalance(userId, -amount, true);
}

int transfer(const char *fromUserId, const char *toUserId, long amount)
{
    if (amount <= 0 || fromUserId == NULL || toUserId == NULL)
    {
        return RESULT_ERROR;
    }

    if (strcmp(fromUserId, toUserId) == 0)
    {
        return RESULT_OK;
    }

    int w = withdrawal(fromUserId, amount);
    if (w != RESULT_OK)
    {
        return w;
    }

    int d = makeDeposit(toUserId, amount);
    if (d != RESULT_OK)
    {
        int rollback = makeDeposit(fromUserId, amount);
        if (rollback != RESULT_OK)
        {
            return RESULT_ERROR;
        }
        return d;
    }

    return RESULT_OK;
}

int makeOperation(operationType operationType, const char *firstUserId, const char *secondUserId, long amount)
{
    if (firstUserId == NULL)
        return RESULT_ERROR;
    if (operationType == TRANSFER && secondUserId == NULL)
        return RESULT_ERROR;

    int res = RESULT_OK;

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

static void freeStoreNewUserMemory(char *userId, char *userName, char *userSurname, char *userAddress, char *userPesel, FILE *fptr)
{
    free(userId);
    free(userName);
    free(userSurname);
    free(userAddress);
    free(userPesel);
    fclose(fptr);
}

void storeNewUser(void)
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
        freeStoreNewUserMemory(userId, userName, userSurname, userAddress, userPesel, fptr);
        return;
    }

    if (!validatePesel(userPesel))
    {
        printf("Wrong pesel format");
        freeStoreNewUserMemory(userId, userName, userSurname, userAddress, userPesel, fptr);
        return;
    }

    int peselLine = findUser(userPesel, PESEL);
    if (peselLine == RESULT_ERROR)
    {
        printf("Could not validate PESEL due to a database error.\n");
        freeStoreNewUserMemory(userId, userName, userSurname, userAddress, userPesel, fptr);
        return;
    }

    if (peselLine >= 0)
    {
        printf("There is a person with such pesel existing in the database! \nThe user was not registered");
        freeStoreNewUserMemory(userId, userName, userSurname, userAddress, userPesel, fptr);
        return;
    }

    printf("Here is your bank account number: %s", userId);
    fprintf(fptr, "%s;%s;%s;%s;%s;%s\n", userId, userName, userSurname, userAddress, userPesel, "0");

    freeStoreNewUserMemory(userId, userName, userSurname, userAddress, userPesel, fptr);
}
