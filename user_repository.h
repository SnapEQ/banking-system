#ifndef USER_REPOSITORY_H
#define USER_REPOSITORY_H

#include <stdbool.h>
#include "banking_types.h"

int findUser(const char *s, searchType type);
int getUserBalance(const char *userId, long *balance);
int updateUserBalance(const char *userId, long delta, bool checkFunds);
void listUser(int lineSearchNum);
void listAllUsers(void);

#endif
