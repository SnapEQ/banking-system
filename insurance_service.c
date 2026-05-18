#include "insurance_service.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "banking_types.h"
#include "common_utils.h"
#include "user_repository.h"

int takeOutInsurance(const char *userId, const char *registrationNo, long amount)
{
    if (!userId || !registrationNo || amount <= 0) return RESULT_ERROR;

    if (findUser(userId, ID) < RESULT_OK) return RESULT_NOT_FOUND;

    FILE *fptr = fopen("ins.txt", "a+");
    if (fptr == NULL) return RESULT_ERROR;

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
            return RESULT_ERROR;
        }

        char *token = strtok(copy, ";");
        token = strtok(NULL, ";");

        if (token != NULL && strcmp(token, registrationNo) == 0)
        {
            free(copy);
            free(line);
            fclose(fptr);
            return RESULT_CONFLICT;
        }

        free(copy);
    }

    free(line);

    if (fprintf(fptr, "%s;%s;%ld\n", userId, registrationNo, amount) < 0)
    {
        fclose(fptr);
        return RESULT_ERROR;
    }

    fclose(fptr);
    return RESULT_OK;
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
