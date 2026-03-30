#include "banking.h"
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


void initBankingSystem() {
    return;
}

char *getUserInput() {
    size_t buffSize = 1024;
    size_t length = 0;

    char *buffer = (char *)malloc(sizeof(char) * buffSize);

    if (buffer == NULL) {
        return NULL;
    }

    int ch;

    while ((ch=getchar()) != '\n') {

        if (length + 1 >= buffSize) {

            buffSize*=2;
            char *tmp = realloc(buffer, buffSize);
            if (tmp == NULL) {
                free(buffer);
                return NULL;
            }

            buffer = tmp;
        }
        buffer[length++] = (char)ch;

    }

    buffer[length] = '\0';

    return buffer;
}

char *getUserName() {
    printf("Provide your name: ");
    char *input = getUserInput();

    if (input == NULL) {
        return NULL;
    }

    return input;
}

char *getrUserSurname() {
    return NULL;
}

char *getUserAddress() {
    return NULL;
}

char *getUserPesel() {
    return NULL;
}

char *getUserBalance() {
    return NULL;
}

char *getUserInsurances() {
    return NULL;
}
