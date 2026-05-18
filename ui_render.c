#include "ui_render.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common_utils.h"
#include "insurance_service.h"

static const char ban[] = " _______                       __        __                     \n|       \\                     |  \\      |  \\                    \n| $$$$$$$\\  ______   _______  | $$   __  \\$$ _______    ______  \n| $$__/ $$ |      \\ |       \\ | $$  /  \\|  \\|       \\  /      \\ \n| $$    $$  \\$$$$$$\\| $$$$$$$\\| $$_/  $$| $$| $$$$$$$\\|  $$$$$$\\\n| $$$$$$$\\ /      $$| $$  | $$| $$   $$ | $$| $$  | $$| $$  | $$\n| $$__/ $$|  $$$$$$$| $$  | $$| $$$$$$\\ | $$| $$  | $$| $$__| $$\n| $$    $$ \\$$    $$| $$  | $$| $$  \\$$\\| $$| $$  | $$ \\$$    $$\n \\$$$$$$$   \\$$$$$$$ \\$$   \\$$ \\$$   \\$$ \\$$ \\$$   \\$$ _\\$$$$$$$\n                                                      |  \\__| $$\n                                                       \\$$    $$\n                                                        \\$$$$$$ \n  ______                         __                             \n /      \\                       |  \\                            \n|  $$$$$$\\ __    __   _______  _| $$_     ______   ______ ____  \n| $$___\\$$|  \\  |  \\ /       \\|   $$ \\   /      \\ |      \\    \\ \n \\$$    \\ | $$  | $$|  $$$$$$$ \\$$$$$$  |  $$$$$$\\| $$$$$$\\$$$$\\\n _\\$$$$$$\\| $$  | $$ \\$$    \\   | $$ __ | $$    $$| $$ | $$ | $$\n|  \\__| $$| $$__/ $$ _\\$$$$$$\\  | $$|  \\| $$$$$$$$| $$ | $$ | $$\n \\$$    $$ \\$$    $$|       $$   \\$$  $$ \\$$     \\| $$ | $$ | $$\n  \\$$$$$$  _\\$$$$$$$ \\$$$$$$$     \\$$$$   \\$$$$$$$ \\$$  \\$$  \\$$\n          |  \\__| $$                                            \n           \\$$    $$                                            \n            \\$$$$$$                                             \n";

void printUserFormatted(const char *line)
{
    if (line == NULL) return;

    char *copy = strdup(line);
    if (copy == NULL) return;

    char *fields[6] = {0};
    int fieldCount = splitUserLine(copy, fields, 6);
    if (fieldCount < 6)
    {
        printf("%s", line);
        free(copy);
        return;
    }

    trimNewline(fields[5]);
    printf("+----------------------------------------+\n");
    printf("| ID      : %-28s |\n", fields[0]);
    printf("| Name    : %-28s |\n", fields[1]);
    printf("| Surname : %-28s |\n", fields[2]);
    printf("| Address : %-28s |\n", fields[3]);
    printf("| PESEL   : %-28s |\n", fields[4]);
    printf("| Balance : %-28s |\n", fields[5]);
    printf("+----------------------------------------+\n");
    printUserInsurances(fields[0]);
    printf("\n");

    free(copy);
}

void clearScreen(void)
{
    printf("\033[2J\033[H");
    fflush(stdout);
}

void waitForEnter(void)
{
    int ch;
    printf("\nPress Enter to continue...");
    fflush(stdout);
    while ((ch = getchar()) != '\n' && ch != EOF)
    {
    }
}

const char *bankingBanner(void)
{
    return ban;
}
