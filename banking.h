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

typedef bool (*Validator)(const char *);

typedef struct {
    searchType type;
    const char *label;
    Validator validate;
} SearchOption;

void initBankingSystem();
int makeDeposit(const char *userId, long amount);
int withdrawl(const char *userId, long amount);
int transfer(const char *fromUserId, const char *toUserId, long amount);
void printMenu();

#endif
