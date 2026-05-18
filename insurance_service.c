#include "insurance_service.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "banking_types.h"
#include "common_utils.h"
#include "user_repository.h"

static bool isInsuranceInputInvalid(const char *userId, const char *registrationNo, long amount)
{
    return userId == NULL || registrationNo == NULL || amount <= 0;
}

static int findRegistrationConflict(FILE *fptr, const char *registrationNo)
{
    char *line = NULL;
    size_t len = 0;

    while (getline(&line, &len, fptr) != -1)
    {
        char *copy = strdup(line);
        if (copy == NULL)
        {
            free(line);
            return RESULT_ERROR;
        }

        char *token = strtok(copy, ";");
        token = strtok(NULL, ";");

        if (token != NULL && strcmp(token, registrationNo) == 0)
        {
            free(copy);
            free(line);
            return RESULT_CONFLICT;
        }

        free(copy);
    }

    free(line);
    return RESULT_OK;
}

static int appendInsurance(FILE *fptr, const char *userId, const char *registrationNo, long amount)
{
    if (fprintf(fptr, "%s;%s;%ld\n", userId, registrationNo, amount) < 0)
        return RESULT_ERROR;
    return RESULT_OK;
}

int takeOutInsurance(const char *userId, const char *registrationNo, long amount)
{
    if (isInsuranceInputInvalid(userId, registrationNo, amount))
        return RESULT_ERROR;

    if (findUser(userId, ID) < RESULT_OK)
        return RESULT_NOT_FOUND;

    FILE *fptr = fopen("ins.txt", "a+");
    if (fptr == NULL)
        return RESULT_ERROR;

    rewind(fptr);
    int conflictCheck = findRegistrationConflict(fptr, registrationNo);
    if (conflictCheck != RESULT_OK)
    {
        fclose(fptr);
        return conflictCheck;
    }

    int appendResult = appendInsurance(fptr, userId, registrationNo, amount);
    fclose(fptr);
    return appendResult;
}

static void printInsuranceHeader(void)
{
    printf("+----------------------------------------+\n");
    printf("| Insurances                             |\n");
    printf("+----------------------------------------+\n");
}

static void printNoInsurance(void)
{
    printf("| %-38s |\n", "none");
    printf("+----------------------------------------+\n");
}

static bool printInsuranceIfOwnedByUser(const char *line, const char *userId)
{
    bool printed = false;
    char *copy = strdup(line);
    if (copy == NULL)
        return false;

    char *insuranceUserId = strtok(copy, ";");
    char *registrationNo = strtok(NULL, ";");
    char *amount = strtok(NULL, ";");

    if (insuranceUserId != NULL && registrationNo != NULL && amount != NULL && strcmp(insuranceUserId, userId) == 0)
    {
        trimNewline(amount);
        printf("| Registration: %-24s |\n", registrationNo);
        printf("| Yearly: %-30s |\n", amount);
        printf("+----------------------------------------+\n");
        printed = true;
    }

    free(copy);
    return printed;
}

void printUserInsurances(const char *userId)
{
    printInsuranceHeader();

    if (userId == NULL)
    {
        printNoInsurance();
        return;
    }

    FILE *fptr = fopen("ins.txt", "r");
    if (fptr == NULL)
    {
        printNoInsurance();
        return;
    }

    char *line = NULL;
    size_t len = 0;
    bool hasInsurance = false;

    while (getline(&line, &len, fptr) != -1)
    {
        if (printInsuranceIfOwnedByUser(line, userId))
            hasInsurance = true;
    }

    if (!hasInsurance)
        printNoInsurance();

    free(line);
    fclose(fptr);
}
