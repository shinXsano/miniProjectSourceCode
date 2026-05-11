#include <stdio.h>
#include <stdlib.h>

struct clientData
{
    unsigned int acctNum;
    char lastName[20];
    char firstName[20];
    double balance;
    unsigned int pin;
};

// Function Prototypes
char enterChoice(void);

void textFile(FILE *fPtr);
void updateRecord(FILE *fPtr);
void newRecord(FILE *fPtr);
void deleteRecord(FILE *fPtr);
void searchRecord(FILE *fPtr);
void displayAll(FILE *fPtr);
void transferMoney(FILE *fPtr);
void statistics(FILE *fPtr);

int authenticate(struct clientData client);

void clearInputBuffer(void)
{
    int c;

    while ((c = getchar()) != '\n' && c != EOF)
    {
    }
}

int main(void)
{
    FILE *cfPtr;
    char choice;

    cfPtr = fopen("credit.dat", "rb+");

    if (cfPtr == NULL)
    {
        cfPtr = fopen("credit.dat", "wb+");

        if (cfPtr == NULL)
        {
            printf("File could not be opened.\n");
            return 1;
        }

        struct clientData blank = {0, "", "", 0.0, 0};

        for (int i = 0; i < 100; i++)
        {
            fwrite(&blank, sizeof(struct clientData), 1, cfPtr);
        }
    }

    while ((choice = enterChoice()) != 'i')
    {
        switch (choice)
        {
        case 'a':
            textFile(cfPtr);
            break;

        case 'b':
            updateRecord(cfPtr);
            break;

        case 'c':
            newRecord(cfPtr);
            break;

        case 'd':
            deleteRecord(cfPtr);
            break;

        case 'e':
            searchRecord(cfPtr);
            break;

        case 'f':
            displayAll(cfPtr);
            break;

        case 'g':
            transferMoney(cfPtr);
            break;

        case 'h':
            statistics(cfPtr);
            break;

        default:
            printf("Invalid choice.\n");
        }
    }

    fclose(cfPtr);

    printf("Program Ended.\n");

    return 0;
}

// MENU
char enterChoice(void)
{
    char choice;

    printf("\n========== BANK MENU ==========\n");
    printf("a - Create text file\n");
    printf("b - Update account\n");
    printf("c - Add new account\n");
    printf("d - Delete account\n");
    printf("e - Search account\n");
    printf("f - Display all accounts\n");
    printf("g - Transfer money\n");
    printf("h - Account statistics\n");
    printf("i - Exit\n");

    printf("Enter choice: ");
    scanf(" %c", &choice);

    clearInputBuffer();

    return choice;
}

// CREATE TEXT FILE
void textFile(FILE *fPtr)
{
    FILE *writePtr;
    struct clientData client;

    writePtr = fopen("accounts.txt", "w");

    if (writePtr == NULL)
    {
        printf("Cannot create text file.\n");
        return;
    }

    rewind(fPtr);

    fprintf(writePtr,
            "%-8s%-15s%-15s%-10s\n",
            "Acct",
            "Last Name",
            "First Name",
            "Balance");

    while (fread(&client,
                 sizeof(struct clientData),
                 1,
                 fPtr))
    {
        if (client.acctNum != 0)
        {
            fprintf(writePtr,
                    "%-8u%-15s%-15s%-10.2f\n",
                    client.acctNum,
                    client.lastName,
                    client.firstName,
                    client.balance);
        }
    }

    fclose(writePtr);

    printf("accounts.txt created successfully.\n");
}

// AUTHENTICATION
int authenticate(struct clientData client)
{
    unsigned int pin;

    printf("Enter PIN: ");
    scanf("%u", &pin);

    clearInputBuffer();

    if (pin == client.pin)
    {
        return 1;
    }

    printf("Incorrect PIN.\n");

    return 0;
}

// ADD NEW ACCOUNT
void newRecord(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0.0, 0};

    unsigned int accountNum;

    printf("Enter new account number (1-100): ");
    scanf("%u", &accountNum);

    clearInputBuffer();

    if (accountNum < 1 || accountNum > 100)
    {
        printf("Invalid account number.\n");
        return;
    }

    fseek(fPtr,
          (long)((accountNum - 1) * sizeof(struct clientData)),
          SEEK_SET);

    fread(&client,
          sizeof(struct clientData),
          1,
          fPtr);

    if (client.acctNum != 0)
    {
        printf("Account already exists.\n");
        return;
    }

    printf("Enter Last Name: ");
    scanf("%19s", client.lastName);

    clearInputBuffer();

    printf("Enter First Name: ");
    scanf("%19s", client.firstName);

    clearInputBuffer();

    printf("Enter Balance: ");
    scanf("%lf", &client.balance);

    clearInputBuffer();

    printf("Create 4-digit PIN: ");
    scanf("%u", &client.pin);

    clearInputBuffer();

    client.acctNum = accountNum;

    fseek(fPtr,
          (long)((accountNum - 1) * sizeof(struct clientData)),
          SEEK_SET);

    fwrite(&client,
           sizeof(struct clientData),
           1,
           fPtr);

    fflush(fPtr);

    printf("Account created successfully.\n");
}

// UPDATE ACCOUNT
void updateRecord(FILE *fPtr)
{
    struct clientData client;

    unsigned int account;

    double amount;

    char option;

    printf("Enter account number: ");
    scanf("%u", &account);

    clearInputBuffer();

    fseek(fPtr,
          (long)((account - 1) * sizeof(struct clientData)),
          SEEK_SET);

    fread(&client,
          sizeof(struct clientData),
          1,
          fPtr);

    if (client.acctNum == 0)
    {
        printf("Account not found.\n");
        return;
    }

    if (!authenticate(client))
    {
        return;
    }

    printf("a - Deposit\n");
    printf("b - Withdraw\n");

    printf("Enter option: ");
    scanf(" %c", &option);

    clearInputBuffer();

    printf("Enter amount: ");
    scanf("%lf", &amount);

    clearInputBuffer();

    if (amount <= 0)
    {
        printf("Invalid amount.\n");
        return;
    }

    if (option == 'a')
    {
        client.balance += amount;
    }
    else if (option == 'b')
    {
        if (amount > client.balance)
        {
            printf("Insufficient balance.\n");
            return;
        }

        client.balance -= amount;
    }
    else
    {
        printf("Invalid option.\n");
        return;
    }

    fseek(fPtr,
          -(long)sizeof(struct clientData),
          SEEK_CUR);

    fwrite(&client,
           sizeof(struct clientData),
           1,
           fPtr);

    fflush(fPtr);

    printf("Updated Balance: %.2f\n",
           client.balance);
}

// DELETE ACCOUNT
void deleteRecord(FILE *fPtr)
{
    struct clientData client;

    struct clientData blank = {0, "", "", 0.0, 0};

    unsigned int accountNum;

    printf("Enter account number to delete: ");
    scanf("%u", &accountNum);

    clearInputBuffer();

    fseek(fPtr,
          (long)((accountNum - 1) * sizeof(struct clientData)),
          SEEK_SET);

    fread(&client,
          sizeof(struct clientData),
          1,
          fPtr);

    if (client.acctNum == 0)
    {
        printf("Account not found.\n");
        return;
    }

    if (!authenticate(client))
    {
        return;
    }

    fseek(fPtr,
          (long)((accountNum - 1) * sizeof(struct clientData)),
          SEEK_SET);

    fwrite(&blank,
           sizeof(struct clientData),
           1,
           fPtr);

    fflush(fPtr);

    printf("Account deleted successfully.\n");
}

// SEARCH ACCOUNT
void searchRecord(FILE *fPtr)
{
    struct clientData client;

    unsigned int account;

    printf("Enter account number: ");
    scanf("%u", &account);

    clearInputBuffer();

    fseek(fPtr,
          (long)((account - 1) * sizeof(struct clientData)),
          SEEK_SET);

    fread(&client,
          sizeof(struct clientData),
          1,
          fPtr);

    if (client.acctNum == 0)
    {
        printf("Account not found.\n");
        return;
    }

    printf("\n===== ACCOUNT DETAILS =====\n");

    printf("Account Number : %u\n",
           client.acctNum);

    printf("Name           : %s %s\n",
           client.firstName,
           client.lastName);

    printf("Balance        : %.2f\n",
           client.balance);
}

// DISPLAY ALL
void displayAll(FILE *fPtr)
{
    struct clientData client;

    rewind(fPtr);

    printf("\n%-8s%-15s%-15s%-10s\n",
           "Acct",
           "Last Name",
           "First Name",
           "Balance");

    while (fread(&client,
                 sizeof(struct clientData),
                 1,
                 fPtr))
    {
        if (client.acctNum != 0)
        {
            printf("%-8u%-15s%-15s%-10.2f\n",
                   client.acctNum,
                   client.lastName,
                   client.firstName,
                   client.balance);
        }
    }
}

// TRANSFER MONEY
void transferMoney(FILE *fPtr)
{
    struct clientData sender;
    struct clientData receiver;

    unsigned int fromAcc;
    unsigned int toAcc;

    double amount;

    printf("Enter sender account: ");
    scanf("%u", &fromAcc);

    clearInputBuffer();

    printf("Enter receiver account: ");
    scanf("%u", &toAcc);

    clearInputBuffer();

    if (fromAcc == toAcc)
    {
        printf("Cannot transfer to same account.\n");
        return;
    }

    fseek(fPtr,
          (long)((fromAcc - 1) * sizeof(struct clientData)),
          SEEK_SET);

    fread(&sender,
          sizeof(struct clientData),
          1,
          fPtr);

    fseek(fPtr,
          (long)((toAcc - 1) * sizeof(struct clientData)),
          SEEK_SET);

    fread(&receiver,
          sizeof(struct clientData),
          1,
          fPtr);

    if (sender.acctNum == 0 || receiver.acctNum == 0)
    {
        printf("One or both accounts not found.\n");
        return;
    }

    if (!authenticate(sender))
    {
        return;
    }

    printf("Enter amount to transfer: ");
    scanf("%lf", &amount);

    clearInputBuffer();

    if (amount <= 0)
    {
        printf("Invalid amount.\n");
        return;
    }

    if (amount > sender.balance)
    {
        printf("Insufficient balance.\n");
        return;
    }

    sender.balance -= amount;
    receiver.balance += amount;

    fseek(fPtr,
          (long)((fromAcc - 1) * sizeof(struct clientData)),
          SEEK_SET);

    fwrite(&sender,
           sizeof(struct clientData),
           1,
           fPtr);

    fseek(fPtr,
          (long)((toAcc - 1) * sizeof(struct clientData)),
          SEEK_SET);

    fwrite(&receiver,
           sizeof(struct clientData),
           1,
           fPtr);

    fflush(fPtr);

    printf("Transfer successful.\n");
}

// STATISTICS
void statistics(FILE *fPtr)
{
    struct clientData client;

    int totalAccounts = 0;

    double totalBalance = 0.0;

    double maxBalance = 0.0;

    rewind(fPtr);

    while (fread(&client,
                 sizeof(struct clientData),
                 1,
                 fPtr))
    {
        if (client.acctNum != 0)
        {
            totalAccounts++;

            totalBalance += client.balance;

            if (client.balance > maxBalance)
            {
                maxBalance = client.balance;
            }
        }
    }

    printf("\n===== BANK STATISTICS =====\n");

    printf("Total Accounts : %d\n",
           totalAccounts);

    printf("Total Balance  : %.2f\n",
           totalBalance);

    if (totalAccounts > 0)
    {
        printf("Average Balance: %.2f\n",
               totalBalance / totalAccounts);
    }

    printf("Highest Balance: %.2f\n",
           maxBalance);
}