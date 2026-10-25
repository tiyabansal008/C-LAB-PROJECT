#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include "storage.h"

// Function to save all system data (Books, Users, Transactions) to binary files
void saveAllData(Book books[], int bookCount, User users[], int userCount, Transaction transactions[], int transCount) {
    // 1. Save Books
    FILE *bookFile = fopen(BOOK_FILE, "wb");
    if (bookFile != NULL) {
        fwrite(&bookCount, sizeof(int), 1, bookFile);
        fwrite(books, sizeof(Book), bookCount, bookFile);
        fclose(bookFile);
    }

    // 2. Save Users
    FILE *userFile = fopen(USER_FILE, "wb");
    if (userFile != NULL) {
        fwrite(&userCount, sizeof(int), 1, userFile);
        fwrite(users, sizeof(User), userCount, userFile);
        fclose(userFile);
    }

    // 3. Save Transactions
    FILE *transFile = fopen(TRANSACTION_FILE, "wb");
    if (transFile != NULL) {
        fwrite(&transCount, sizeof(int), 1, transFile);
        fwrite(transactions, sizeof(Transaction), transCount, transFile);
        fclose(transFile);
    }
}

// Function to load all system data from binary files on application startup
void loadAllData(Book books[], int *bookCount, User users[], int *userCount, Transaction transactions[], int *transCount) {
    // 1. Load Books
    FILE *bookFile = fopen(BOOK_FILE, "rb");
    if (bookFile != NULL) {
        fread(bookCount, sizeof(int), 1, bookFile);
        fread(books, sizeof(Book), *bookCount, bookFile);
        fclose(bookFile);
    } else {
        *bookCount = 0;
    }

    // 2. Load Users
    FILE *userFile = fopen(USER_FILE, "rb");
    if (userFile != NULL) {
        fread(userCount, sizeof(int), 1, userFile);
        fread(users, sizeof(User), *userCount, userFile);
        fclose(userFile);
    } else {
        *userCount = 0;
    }

    // 3. Load Transactions
    FILE *transFile = fopen(TRANSACTION_FILE, "rb");
    if (transFile != NULL) {
        fread(transCount, sizeof(int), 1, transFile);
        fread(transactions, sizeof(Transaction), *transCount, transFile);
        fclose(transFile);
    } else {
        *transCount = 0;
    }
}