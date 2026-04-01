#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "banking.h"

int main(int argc, char *argv[])
{
    srand((unsigned)time(NULL));
    storeNewUser();
    return 0;
}
