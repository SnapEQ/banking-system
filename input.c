#include "input.h"

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "banking_types.h"
#include "common_utils.h"

static char *allocateInputBuffer(size_t *bufferSize)
{
    *bufferSize = 1024;
    return (char *)malloc(*bufferSize * sizeof(char));
}

static int readSanitizedInput(char **buffer, size_t *bufferSize, size_t *length, int *lastChar)
{
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF)
    {
        if ((char)ch == ';')
        {
            continue;
        }

        if (appendToDynamicBuffer(buffer, bufferSize, length, (char)ch) != RESULT_OK)
        {
            return RESULT_ERROR;
        }
    }

    *lastChar = ch;
    return RESULT_OK;
}

char *getUserInput(size_t *length)
{
    size_t buffSize = 0;
    *length = 0;

    char *buffer = allocateInputBuffer(&buffSize);
    if (buffer == NULL)
    {
        return NULL;
    }

    int ch = '\n';
    if (readSanitizedInput(&buffer, &buffSize, length, &ch) != RESULT_OK)
    {
        free(buffer);
        return NULL;
    }

    if (ch == EOF && *length == 0)
    {
        free(buffer);
        return NULL;
    }

    buffer[*length] = '\0';

    return buffer;
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
