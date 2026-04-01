#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "banking.h"

int main(int argc, char *argv[])
{
    srand((unsigned)time(NULL));
    int k = findUser("8ii3", ID);
    printf("\n%d\n", k);
    return 0;
}
