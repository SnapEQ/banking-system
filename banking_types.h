#ifndef BANKING_TYPES_H
#define BANKING_TYPES_H

#include <stdbool.h>

typedef enum{
    ID,
    NAME,
    SURNAME,
    ADDRESS,
    PESEL
} searchType;

typedef enum{
    DEPOSIT,
    WITHDRAWAL,
    TRANSFER
} operationType;

typedef enum{
    RESULT_OK = 0,
    RESULT_NOT_FOUND = -1,
    RESULT_ERROR = -2,
    RESULT_CONFLICT = -3
} ResultCode;

typedef bool (*Validator)(const char *);

typedef struct {
    searchType type;
    const char *label;
    Validator validate;
} SearchOption;

typedef struct {
    operationType type;
    const char *label;
    Validator validate;
} OperationOption;

#endif
