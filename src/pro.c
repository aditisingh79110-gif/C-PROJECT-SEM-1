/* bank.c
   Basic Bank Account Management System (Version A)
   Features:
   - Create account (with uniqueness check)
   - Deposit money
   - Withdraw money (with simple insufficient balance check)
   - Check balance
   - Persistent storage using binary file "accounts.dat"
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Account structure */
typedef struct {
    long acc_no;
    char name[100];
    float balance;
} Account;

/* File name */
const char *FILENAME = "accounts.dat";

/* Function prototypes */
int accountExists(long acc_no);
void createAccount();
void deposit();
void withdrawAmount();
void checkBalance();
void pauseClear();

int main() {
    int choice;
    printf("=== Basic Bank Account Management System ===\n");
    while (1) {
        printf("\nMenu:\n");
        printf("1. Create Account\n");
        printf("2. Deposit\n");
        printf("3. Withdraw\n");
        printf("4. Check Balance\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Exiting.\n");
            break;
        }
        switch (choice) {
            case 1: createAccount(); break;
            case 2: deposit(); break;
            case 3: withdrawAmount(); break;
            case 4: checkBalance(); break;
            case 5: printf("Goodbye!\n"); exit(0);
            default: printf("Invalid choice. Try again.\n");
        }
    }
    return 0;
}
/* Check if account exists; returns 1 if exists, else 0 */
int accountExists(long acc_no) {
    FILE *fp = fopen(FILENAME, "rb");
    if (!fp) return 0; /* file doesn't exist yet -> no accounts */
    Account acc;
    while (fread(&acc, sizeof(Account), 1, fp) == 1) {
        if (acc.acc_no == acc_no) {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

/* Create a new account and save to file */
void createAccount() {
    Account acc;
    printf("\n--- Create Account ---\n");
    printf("Enter account number (numeric): ");
    if (scanf("%ld", &acc.acc_no) != 1) {
        printf("Invalid account number input.\n");
        return;
    }
    if (accountExists(acc.acc_no)) {
        printf("Account with number %ld already exists.\n", acc.acc_no);
        return;
    }
    printf("Enter account holder name: ");
    getchar(); /* consume newline */
    fgets(acc.name, sizeof(acc.name), stdin);
    /* Remove trailing newline if present */
    size_t ln = strlen(acc.name);
    if (ln && acc.name[ln-1] == '\n') acc.name[ln-1] = '\0';
    printf("Enter initial deposit amount: ");
    if (scanf("%f", &acc.balance) != 1 || acc.balance < 0) {
        printf("Invalid amount. Account creation aborted.\n");
        return;
    }

    /* Save to file (append) */
    FILE *fp = fopen(FILENAME, "ab");
    if (!fp) {
        printf("Error opening file for writing.\n");
        return;
    }
    fwrite(&acc, sizeof(Account), 1, fp);
    fclose(fp);
    printf("Account created successfully!\nAccount No: %ld\nName: %s\nBalance: %.2f\n",
           acc.acc_no, acc.name, acc.balance);
}

/* Deposit money into account */
void deposit() {
    long acc_no;
    float amt;
    printf("\n--- Deposit ---\n");
    printf("Enter account number: ");
    if (scanf("%ld", &acc_no) != 1) {
        printf("Invalid input.\n");
        return;
    }
    printf("Enter amount to deposit: ");
    if (scanf("%f", &amt) != 1 || amt <= 0) {
        printf("Invalid amount.\n");
        return;
    }

    FILE *fp = fopen(FILENAME, "rb+");
    if (!fp) {
        printf("No accounts exist yet.\n");
        return;
    }
    Account acc;
    int found = 0;
    while (fread(&acc, sizeof(Account), 1, fp) == 1) {
        if (acc.acc_no == acc_no) {
            found = 1;
            acc.balance += amt;
            /* Move file pointer back one record and overwrite */
            fseek(fp, - (long)sizeof(Account), SEEK_CUR);
            fwrite(&acc, sizeof(Account), 1, fp);
            printf("Deposit successful. New balance: %.2f\n", acc.balance);
            break;
        }
    }
    if (!found) printf("Account number %ld not found.\n", acc_no);
    fclose(fp);
}

/* Withdraw money from account */
void withdrawAmount() {
    long acc_no;
    float amt;
    printf("\n--- Withdraw ---\n");
    printf("Enter account number: ");
    if (scanf("%ld", &acc_no) != 1) {
        printf("Invalid input.\n");
        return;
    }
    printf("Enter amount to withdraw: ");
    if (scanf("%f", &amt) != 1 || amt <= 0) {
        printf("Invalid amount.\n");
        return;
    }

    FILE *fp = fopen(FILENAME, "rb+");
    if (!fp) {
        printf("No accounts exist yet.\n");
        return;
    }
    Account acc;
    int found = 0;
    while (fread(&acc, sizeof(Account), 1, fp) == 1) {
        if (acc.acc_no == acc_no) {
            found = 1;
            if (acc.balance < amt) {
                printf("Insufficient balance. Current balance: %.2f\n", acc.balance);
            } else {
                acc.balance -= amt;
                fseek(fp, - (long)sizeof(Account), SEEK_CUR);
                fwrite(&acc, sizeof(Account), 1, fp);
                printf("Withdrawal successful. New balance: %.2f\n", acc.balance);
            }
            break;
        }
    }
    if (!found) printf("Account number %ld not found.\n", acc_no);
    fclose(fp);
}

/* Check balance for an account */
void checkBalance() {
    long acc_no;
    printf("\n--- Check Balance ---\n");
    printf("Enter account number: ");
    if (scanf("%ld", &acc_no) != 1) {
        printf("Invalid input.\n");
        return;
    }
    FILE *fp = fopen(FILENAME, "rb");
    if (!fp) {
        printf("No accounts exist yet.\n");
        return;
    }
    Account acc;
    int found = 0;
    while (fread(&acc, sizeof(Account), 1, fp) == 1) {
        if (acc.acc_no == acc_no) {
            found = 1;
            printf("Account No: %ld\nName: %s\nBalance: %.2f\n",
                   acc.acc_no, acc.name, acc.balance);
            break;
        }
    }
    if (!found) printf("Account number %ld not found.\n", acc_no);
    fclose(fp);
}
