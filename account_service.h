#ifndef ACCOUNT_SERVICE_H
#define ACCOUNT_SERVICE_H

#include "banking_types.h"

int makeDeposit(const char *userId, long amount);
int withdrawal(const char *userId, long amount);
int transfer(const char *fromUserId, const char *toUserId, long amount);
int makeOperation(operationType operationType, const char *firstUserId, const char *secondUserId, long amount);
void storeNewUser(void);

#endif
