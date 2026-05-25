#include "user_repository.h"

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common_utils.h"
#include "ui_render.h"

static int semicolonLimitForSearchType(searchType type)
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
        return RESULT_ERROR;
    }
}

static int extractSearchField(const char *line, int semicolonLimit, char **outField)
{
    size_t index = 0;
    int semicolonCount = 0;
    size_t bufferSize = 256;
    size_t bufferIndex = 0;
    char *buffer = malloc(bufferSize * sizeof(char));

    if (buffer == NULL)
    {
        return RESULT_ERROR;
    }

    while (semicolonCount < semicolonLimit && line[index] != '\0')
    {
        if (semicolonCount == semicolonLimit - 1 && line[index] != ';')
        {
            if (appendToDynamicBuffer(&buffer, &bufferSize, &bufferIndex, line[index]) != RESULT_OK)
            {
                free(buffer);
                return RESULT_ERROR;
            }
        }

        if (line[index] == ';') semicolonCount++;
        index++;
    }

    if (semicolonCount < semicolonLimit)
    {
        free(buffer);
        return RESULT_OK;
    }

    buffer[bufferIndex] = '\0';
    *outField = buffer;
    return 1;
}

static int doesLineMatchSearch(const char *line, const char *value, int semicolonLimit)
{
    char *field = NULL;
    int extractionResult = extractSearchField(line, semicolonLimit, &field);

    if (extractionResult <= RESULT_OK)
    {
        return extractionResult;
    }

    int isMatch = strcmp(value, field) == 0;
    free(field);
    return isMatch ? 1 : RESULT_OK;
}

static int calculateNextBalance(const char *balanceField, long delta, bool checkFunds, long *nextBalance)
{
    if (balanceField == NULL || nextBalance == NULL) return RESULT_ERROR;

    if (balanceField[0] == '\0') return RESULT_ERROR;

    char *endptr = NULL;
    errno = 0;
    long currentBalance = strtol(balanceField, &endptr, 10);
    if (endptr == balanceField || errno != 0) return RESULT_ERROR;

    while (*endptr != '\0')
    {
        if (!isspace((unsigned char)*endptr)) return RESULT_ERROR;
        endptr++;
    }

    if (currentBalance < 0) return RESULT_ERROR;

    if (delta > 0 && currentBalance > LONG_MAX - delta) return RESULT_ERROR;

    if (delta < 0 && currentBalance < LONG_MIN - delta) return RESULT_ERROR;

    *nextBalance = currentBalance + delta;
    if (checkFunds && *nextBalance < 0) return RESULT_CONFLICT;

    return RESULT_OK;
}

static void writeUserLineWithNewBalance(FILE *dst, char **fields, long nextBalance)
{
    fprintf(dst, "%s;%s;%s;%s;%s;%ld\n",
            fields[0],
            fields[1],
            fields[2],
            fields[3],
            fields[4],
            nextBalance);
}

static int processBalanceLine(FILE *dst, const char *line, const char *userId, long delta, bool checkFunds, bool *updated)
{
    char *copy = strdup(line);
    if (copy == NULL)
    {
        return RESULT_ERROR;
    }

    char *fields[6] = {0};
    int fieldCount = splitUserLine(copy, fields, 6);

    if (fieldCount >= 6 && strcmp(fields[0], userId) == 0)
    {
        long nextBalance = 0;
        int calcResult = calculateNextBalance(fields[5], delta, checkFunds, &nextBalance);
        if (calcResult != RESULT_OK)
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
    return RESULT_OK;
}

int findUser(const char *s, searchType type)
{
    if (s == NULL) return RESULT_ERROR;

    int semicolonLimit = semicolonLimitForSearchType(type);
    if (semicolonLimit < RESULT_OK) return RESULT_ERROR;

    FILE *fptr = fopen("db.txt", "r");
    if (fptr == NULL) return RESULT_ERROR;

    char *line = NULL;
    size_t len = 0;
    int lineNum = 0;

    while (getline(&line, &len, fptr) != -1)
    {
        int lineMatchResult = doesLineMatchSearch(line, s, semicolonLimit);
        if (lineMatchResult == RESULT_ERROR)
        {
            free(line);
            fclose(fptr);
            return RESULT_ERROR;
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
    return RESULT_NOT_FOUND;
}

int updateUserBalance(const char *userId, long delta, bool checkFunds)
{
    if (userId == NULL) return RESULT_ERROR;

    FILE *src = fopen("db.txt", "r");
    FILE *dst = fopen("db.tmp", "w");

    if (src == NULL || dst == NULL)
    {
        if (src != NULL) fclose(src);
        if (dst != NULL) fclose(dst);
        return RESULT_ERROR;
    }

    char *line = NULL;
    size_t len = 0;
    bool found = false;

    while (getline(&line, &len, src) != -1)
    {
        int lineResult = processBalanceLine(dst, line, userId, delta, checkFunds, &found);
        if (lineResult != RESULT_OK)
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
        return RESULT_NOT_FOUND;
    }

    if (rename("db.tmp", "db.txt") != 0)
    {
        remove("db.tmp");
        return RESULT_ERROR;
    }

    return RESULT_OK;
}

void listUser(int lineSearchNum)
{
    FILE *fptr = fopen("db.txt", "r");

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

void listAllUsers(void)
{
    FILE *fptr = fopen("db.txt", "r");

    if (fptr == NULL) return;

    char *line = NULL;
    size_t len = 0;

    while (getline(&line, &len, fptr) != -1)
    {
        printUserFormatted(line);
    }

    free(line);
    fclose(fptr);
}
