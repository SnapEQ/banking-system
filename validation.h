#ifndef VALIDATION_H
#define VALIDATION_H

#include <stddef.h>
#include <stdbool.h>

bool validateNonEmpty(const char *s);
bool validateId(const char *s);
bool validatePesel(const char *s);
char *getRegistrationNo(size_t *length);

#endif
