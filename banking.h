#ifndef BANKING_H
#define BANKING_H
#include <stddef.h>

void initBankingSystem();
char *getUserName(size_t* length);
int findUserByName(const char *name);
void storeNewUser();

#endif
