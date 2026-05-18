#ifndef INPUT_H
#define INPUT_H

#include <stddef.h>

char *getUserInput(size_t *length);
int readIntInRange(const char *prompt, int min, int max);
long readLongInRange(const char *prompt, long min, long max);
char readConfirmation(const char *prompt, char def);

#endif
