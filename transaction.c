#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "transaction.h"

// Function to issue a book to a user
void issueBook(Book books[], int bookCount, User users[], int userCount, Transaction transactions[], int *transCount) {
    if (*transCount >= MAX_TRANSACTIONS) {
        printf("\n[Error] Transaction database is full.\n");
        return;
    }

    int bID, uID;
    printf("\n==============================\n");
    printf("         ISSUE A BOOK         \n");
    printf("==============================\n");
    printf("Enter User ID: ");
    scanf("%d", &uID);

    // Verify if user exists
    int userFound = 0;
    for (int i = 0; i < userCount; i++) {
        if (users[i].id == uID) {
            userFound = 1;
            break;
        }
    }
    if (!userFound) {
        printf("\n[Error] User ID not found. Please register first.\n");
        return;
    }

    printf("Enter Book ID to issue: ");
    scanf("%d", &bID);

    // Find book and check stock
    int bookIndex = -1;
    for (int i = 0; i < bookCount; i++) {
        if (books[i].id == bID) {
            bookIndex = i;
            break;
        }
    }

    if (bookIndex == -1) {
        printf("\n[Error] Book ID not found in inventory.\n");
        return;
    }

    if (books[bookIndex].quantity <= 0) {
        printf("\n[Error] Book is currently out of stock!\n");
        return;
    }

    // Decrease book quantity
    books[bookIndex].quantity--;

    // Record transaction details
    transactions[*transCount].bookID = bID;
    transactions[*transCount].userID = uID;
    transactions[*transCount].issueDate = time(NULL);
    
    // Set due date to 14 days from today (14 days * 24 hours * 60 mins * 60 secs)
    transactions[*transCount].dueDate = transactions[*transCount].issueDate + (14 * 24 * 60 * 60);
    transactions[*transCount].isReturned = 0; // Active loan

    (*transCount)++;
    
    // Save updated records
    saveBooksToFile(books, bookCount);
    saveTransactionsToFile(transactions, *transCount);

    printf("\n[Success] Book issued successfully! Due in 14 days.\n");
}

// Function to process a book return and calculate fines
void returnBook(Book books[], int bookCount, Transaction transactions[], int transCount) {
    int bID, uID;
    printf("\n==============================\n");
    printf("         RETURN A BOOK        \n");
    printf("==============================\n");
    printf("Enter User ID: ");
    scanf("%d", &uID);
    printf("Enter Book ID being returned: ");
    scanf("%d", &bID);

    int transIndex = -1;
    for (int i = 0; i < transCount; i++) {
        if (transactions[i].bookID == bID && transactions[i].userID == uID && transactions[i].isReturned == 0) {
            transIndex = i;
            break;
        }
    }

    if (transIndex == -1) {
        printf("\n[Error] No active loan found matching this User ID and Book ID.\n");
        return;
    }

    // Mark as returned
    transactions[transIndex].isReturned = 1;

    // Restore book stock in inventory
    for (int i = 0; i < bookCount; i++) {
        if (books[i].id == bID) {
            books[i].quantity++;
            break;
        }
    }

    // Calculate fine if overdue
    time_t currentTime = time(NULL);
    double secondsLate = difftime(currentTime, transactions[transIndex].dueDate);
    
    if (secondsLate > 0) {
        // Convert seconds to days (1 day = 86400 seconds)
        int daysLate = (int)(secondsLate / (24 * 60 * 60)) + 1;
        int finePerDay = 5; // Rs. 5 fine per overdue day
        int totalFine = daysLate * finePerDay;
        printf("\n[Notice] Book is OVERDUE by %d day(s).\n", daysLate);
        printf("Total Fine to Collect: Rs. %d\n", totalFine);
    } else {
        printf("\n[Success] Book returned on time! No fine incurred.\n");
    }

    // Save changes
    saveBooksToFile(books, bookCount);
    saveTransactionsToFile(transactions, transCount);
    printf("[Success] Return processed successfully!\n");
}

// Function to view all active (unreturned) loans
void viewActiveLoans(Transaction transactions[], int transCount) {
    int activeCount = 0;
    for (int i = 0; i < transCount; i++) {
        if (transactions[i].isReturned == 0) {
            activeCount++;
        }
    }

    if (activeCount == 0) {
        printf("\n[Info] No active loans currently recorded.\n");
        return;
    }

    printf("\n=============================================================\n");
    printf("                    ACTIVE LOANS DIRECTORY                   \n");
    printf("=============================================================\n");
    printf("%-10s | %-10s | %-30s\n", "User ID", "Book ID", "Due Date");
    printf("-------------------------------------------------------------\n");

    for (int i = 0; i < transCount; i++) {
        if (transactions[i].isReturned == 0) {
            // Format time string nicely
            char timeBuffer[30];
            struct tm *timeinfo = localtime(&transactions[i].dueDate);
            strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d", timeinfo);

            printf("%-10d | %-10d | %-30s\n", 
                   transactions[i].userID, transactions[i].bookID, timeBuffer);
        }
    }
    printf("=============================================================\n");
}

// Function to save transactions to binary file
void saveTransactionsToFile(Transaction transactions[], int transCount) {
    FILE *file = fopen(TRANSACTION_FILE, "wb");
    if (file == NULL) {
        return;
    }
    fwrite(&transCount, sizeof(int), 1, file);
    fwrite(transactions, sizeof(Transaction), transCount, file);
    fclose(file);
}

// Function to load transactions from binary file on startup
int loadTransactionsFromFile(Transaction transactions[]) {
    FILE *file = fopen(TRANSACTION_FILE, "rb");
    if (file == NULL) {
        return 0; // No file exists yet
    }
    int count = 0;
    fread(&count, sizeof(int), 1, file);
    fread(transactions, sizeof(Transaction), count, file);
    fclose(file);
    return count;
}