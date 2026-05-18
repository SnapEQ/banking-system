#include "validation.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "input.h"

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
