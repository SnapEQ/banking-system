#ifndef BANKING_H
#define BANKING_H
#include <stddef.h>
#include <stdbool.h>

typedef enum{
    ID,
    NAME,
    SURNAME,
    ADDRESS,
    PESEL
}searchType;

typedef enum{
    DEPOSIT,
    WITHDRAWAL,
    TRANSFER
}operationType;

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

void initBankingSystem();
void printMenu();
int makeDeposit(const char *userId, long amount);
int withdrawl(const char *userId, long amount);
int transfer(const char *fromUserId, const char *toUserId, long amount);
int makeOperation(operationType operationType, const char *firstUserId, const char *secondUserId, long amount);

#endif
