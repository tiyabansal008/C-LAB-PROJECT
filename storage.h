#ifndef STORAGE_H
#define STORAGE_H

#include "book.h"
#include "auth.h"
#include "transaction.h"

// File Names
#define BOOK_FILE "books_data.dat"
#define USER_FILE "users_data.dat"
#define TRANSACTION_FILE "transactions_data.dat"

// Centralized Persistence Function Prototypes
void saveAllData(Book books[], int bookCount, User users[], int userCount, Transaction transactions[], int transCount);
void loadAllData(Book books[], int *bookCount, User users[], int *userCount, Transaction transactions[], int *transCount);

#endif