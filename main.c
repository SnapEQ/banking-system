#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "banking.h"

int main(int argc, char *argv[])
{
    srand((unsigned)time(NULL));
   // storeNewUser();
    listAllUsers();
    //int k = findUser("06321204451", PESEL);
    //printf("\n%d\n", k);
    return 0;
}
