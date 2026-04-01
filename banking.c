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
    printf("%s", userId);
    fprintf(fptr, "%s;%s;%s;%s;%s;%s;%s\n", userId, userName, userSurname, userAddress, userPesel, "0", "0");
 
    free(userId);
    free(userName);
    free(userSurname);
    free(userAddress);
    free(userPesel);
    fclose(fptr);
}
