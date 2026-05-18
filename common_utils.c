#include "common_utils.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "banking_types.h"

int ensureBufferCapacity(char **buffer, size_t *bufferSize, size_t requiredIndex)
{
    if (requiredIndex + 1 < *bufferSize)
    {
        return RESULT_OK;
    }

    size_t nextSize = *bufferSize;
    while (requiredIndex + 1 >= nextSize)
    {
        if (nextSize > SIZE_MAX / 2)
        {
            return RESULT_ERROR;
        }
        nextSize *= 2;
    }

    char *tmp = realloc(*buffer, nextSize);
    if (tmp == NULL)
    {
        return RESULT_ERROR;
    }

    *buffer = tmp;
    *bufferSize = nextSize;
    return RESULT_OK;
}

int appendToDynamicBuffer(char **buffer, size_t *bufferSize, size_t *bufferIndex, char c)
{
    if (ensureBufferCapacity(buffer, bufferSize, *bufferIndex) != RESULT_OK)
    {
        return RESULT_ERROR;
    }

    (*buffer)[(*bufferIndex)++] = c;
    return RESULT_OK;
}

int splitUserLine(char *line, char **fields, int maxFields)
{
    int i = 0;
    char *token = strtok(line, ";");

    while (token != NULL && i < maxFields)
    {
        fields[i++] = token;
        token = strtok(NULL, ";");
    }

    return i;
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
