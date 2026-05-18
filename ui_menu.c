#include "ui_menu.h"

#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "account_service.h"
#include "banking_types.h"
#include "input.h"
#include "insurance_service.h"
#include "ui_render.h"
#include "user_repository.h"
#include "validation.h"

static const SearchOption SEARCH_OPTIONS[] = {
    {ID, "ID", validateId},
    {NAME, "Name", validateNonEmpty},
    {SURNAME, "Surname", validateNonEmpty},
    {ADDRESS, "Address", validateNonEmpty},
    {PESEL, "PESEL", validatePesel},
};

static int searchOptionsCount(void)
{
    return (int)(sizeof(SEARCH_OPTIONS) / sizeof(SEARCH_OPTIONS[0]));
}

static const OperationOption OPERATION_OPTIONS[] = {
    {DEPOSIT, "Deposit", validateId},
    {WITHDRAWAL, "Withdrawal", validateId},
    {TRANSFER, "Transfer", validateId},
};

static int operationOptionsCount(void)
{
    return (int)(sizeof(OPERATION_OPTIONS) / sizeof(OPERATION_OPTIONS[0]));
}

static char *readValidatedValue(const SearchOption *opt)
{
    while (1)
    {
        size_t len = 0;
        printf("Enter %s (0 to go back): ", opt->label);
        fflush(stdout);

        char *value = getUserInput(&len);
        if (!value)
            return NULL;
        if (strcmp(value, "0") == 0)
        {
            free(value);
            return NULL;
        }

        if (opt->validate(value))
            return value;

        printf("Invalid %s format.\n", opt->label);
        free(value);
    }
}

static char *readValidatedValueOperation(const OperationOption *opt)
{
    while (1)
    {
        size_t len = 0;
        printf("Enter Id (0 to go back): ");
        fflush(stdout);

        char *value = getUserInput(&len);
        if (!value)
            return NULL;
        if (strcmp(value, "0") == 0)
        {
            free(value);
            return NULL;
        }

        if (opt->validate(value))
            return value;

        printf("Invalid ID format.\n");
        free(value);
    }
}

static char *readValidatedID(void)
{
    while (1)
    {
        size_t len = 0;
        printf("Enter Id (0 to go back): ");
        fflush(stdout);

        char *value = getUserInput(&len);
        if (!value)
            return NULL;
        if (strcmp(value, "0") == 0)
        {
            free(value);
            return NULL;
        }

        if (validateId(value))
            return value;

        printf("Invalid ID format.\n");
        free(value);
    }
}

static void insuranceSubmenu(void)
{
    while (true)
    {
        clearScreen();
        printf("=== Insurances ===\n");
        printf("1. Take out insurance\n");
        printf("0. Back\n\n");

        int choice = readIntInRange("Choose: ", 0, 1);

        if (choice == 0) return;

        if (choice == 1)
        {
            char *userId = readValidatedID();
            if (!userId)
            {
                continue;
            }

            size_t length = 0;
            char *registrationNo = NULL;
            registrationNo = getRegistrationNo(&length);
            long amount = readLongInRange("Type in the amount of the yearly payment: ", 1, LONG_MAX);

            int res = takeOutInsurance(userId, registrationNo, amount);

            if (!registrationNo || res < RESULT_OK)
            {
                if (!registrationNo)
                {
                    printf("Invalid registration number format.\n");
                }
                else if (res == RESULT_NOT_FOUND)
                {
                    printf("User ID not found in database.\n");
                }
                else if (res == RESULT_CONFLICT)
                {
                    printf("Insurance already exists for this registration number.\n");
                }
                else
                {
                    printf("Failed to take out insurance. Please try again.\n");
                }

                free(userId);
                free(registrationNo);
                waitForEnter();
                continue;
            }

            printf("Insurance has been registered for user: %s\n", userId);
            free(userId);
            free(registrationNo);
            waitForEnter();
        }
    }
}

static void listUserSubmenu(void)
{
    while (true)
    {
        clearScreen();
        printf("=== List Users By ===\n");
        for (int i = 0; i < searchOptionsCount(); i++)
        {
            printf("%d. %s\n", i + 1, SEARCH_OPTIONS[i].label);
        }
        printf("%d. All Users\n", searchOptionsCount() + 1);
        printf("0. Back\n\n");

        int choice = readIntInRange("Choose: ", 0, searchOptionsCount() + 1);

        if (choice == 0)
            return;

        if (choice == searchOptionsCount() + 1)
        {
            listAllUsers();
            waitForEnter();
            continue;
        }

        const SearchOption *opt = &SEARCH_OPTIONS[choice - 1];
        char *value = readValidatedValue(opt);
        if (!value)
            continue;

        int line = findUser(value, opt->type);
        if (line >= 0)
            listUser(line);
        else
            printf("User not found\n");

        free(value);
        waitForEnter();
    }
}

static void operationsSubmenu(void)
{
    while (true)
    {
        clearScreen();
        printf("=== Operations ===\n");
        for (int i = 0; i < operationOptionsCount(); i++)
        {
            printf("%d. %s\n", i + 1, OPERATION_OPTIONS[i].label);
        }
        printf("0. Back\n\n");

        int choice = readIntInRange("Choose: ", 0, operationOptionsCount());

        if (choice == 0)
            return;

        const OperationOption *opt = &OPERATION_OPTIONS[choice - 1];

        char *userIdValidated = NULL;
        char *secondUserIdValidated = NULL;

        userIdValidated = readValidatedValueOperation(opt);
        if (!userIdValidated)
            continue;

        if (opt->type == TRANSFER)
        {
            secondUserIdValidated = readValidatedValueOperation(opt);
            if (!secondUserIdValidated)
            {
                printf("Second user accountNumber is required for transfer.\n");
                free(userIdValidated);
                waitForEnter();
                continue;
            }
        }

        long amount = readLongInRange("Enter amount: ", 1, LONG_MAX);

        if (amount == 0L)
        {
            printf("Line parsing went wrong, Try again\n");
        }
        else if (readConfirmation("Do you want to proceed? (Y/n): ", 'n') == 'y')
        {
            int res = makeOperation(opt->type, userIdValidated, secondUserIdValidated, amount);
            if (res < RESULT_OK)
            {
                printf("Operation failed\n");
                free(userIdValidated);
                free(secondUserIdValidated);
                waitForEnter();
                continue;
            }
        }

        free(secondUserIdValidated);
        free(userIdValidated);
        waitForEnter();
    }
}

void printMenu(void)
{
    while (true)
    {
        clearScreen();
        printf("%s\n", bankingBanner());
        printf("List of operations\n");
        printf("1 List user\n");
        printf("2 Operations\n");
        printf("3 Register account\n");
        printf("4 Insurances\n");
        printf("0 Exit\n\n");

        int choice = readIntInRange("Choose: ", 0, 4);

        switch (choice)
        {
        case 1:
            listUserSubmenu();
            break;
        case 2:
            operationsSubmenu();
            break;
        case 3:
            storeNewUser();
            waitForEnter();
            break;
        case 4:
            insuranceSubmenu();
            break;
        case 0:
            return;
        default:
            break;
        }
    }
}
