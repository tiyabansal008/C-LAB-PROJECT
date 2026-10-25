#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <time.h>
#include "book.h"
#include "auth.h"

#define MAX_TRANSACTIONS 100
#define TRANSACTION_FILE "transactions_data.dat"

// Structure to represent a borrowing transaction
typedef struct {
    int bookID;
    int userID;
    time_t issueDate;
    time_t dueDate;
    int isReturned; // 0 = Active Loan, 1 = Returned
} Transaction;

// Function Prototypes
void issueBook(Book books[], int bookCount, User users[], int userCount, Transaction transactions[], int *transCount);
void returnBook(Book books[], int bookCount, Transaction transactions[], int transCount);
void viewActiveLoans(Transaction transactions[], int transCount);
void saveTransactionsToFile(Transaction transactions[], int transCount);
int loadTransactionsFromFile(Transaction transactions[]);

#endif