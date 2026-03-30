#include <stdlib.h>
#include <stdio.h>
#include "banking.h"

int main(int argc, char *argv[])
{
    char *k = getUserName();

    printf("%s\n", k);

    free(k);
    return 0;
}
