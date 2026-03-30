#include "banking.h"
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

// TO DO
//  IMPLEMENT ID GENERATOR
void storeNewUser()
{
    FILE *fptr;

    fptr = fopen("db.txt", "w");
    if (fptr == NULL)
    {
        return;
    }

    size_t length = 0;
    char *userName = getUserName(&length);
    char *userSurname = getUserSurname(&length);
    char *userAddress = getUserAddress(&length);
    char *userPesel = getUserPesel(&length);

    if (userName == NULL || userSurname == NULL || userAddress == NULL || userPesel == NULL)
    {
        free(userName);
        free(userSurname);
        free(userAddress);
        free(userPesel);
        fclose(fptr);
        return;
    }

    fprintf(fptr, "%s;%s;%s;%s;%s;%s", userName, userSurname, userAddress, userPesel, "0", "0");

    free(userName);
    free(userSurname);
    free(userAddress);
    free(userPesel);
    fclose(fptr);
}
