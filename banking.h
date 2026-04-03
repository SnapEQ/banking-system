#ifndef BANKING_H
#define BANKING_H
#include <stddef.h>

typedef enum{
    ID,
    NAME,
    SURNAME,
    ADDRESS,
    PESEL
}searchType;

void initBankingSystem();
int findUser(const char *s, searchType type);
void storeNewUser();
void listAllUsers();

#endif
