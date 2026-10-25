#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include "auth.h"
#include "book.h"
#include "transaction.h"
#include "storage.h"

// Function prototypes for navigation dashboards
void adminDashboard(Book books[], int *bookCount, User users[], int *userCount, Transaction transactions[], int *transCount);
void memberDashboard(Book books[], int bookCount, User users[], int userCount, Transaction transactions[], int *transCount);

int main() {
    Book books[MAX_BOOKS];
    int bookCount = 0;

    User users[MAX_USERS];
    int userCount = 0;

    Transaction transactions[MAX_TRANSACTIONS];
    int transCount = 0;

    // Load all persistent data from binary files on startup
    loadAllData(books, &bookCount, users, &userCount, transactions, &transCount);

    int choice;
    do {
        printf("\n=========================================\n");
        printf("       LIBRARY MANAGEMENT SYSTEM         \n");
        printf("=========================================\n");
        printf("1. Admin Portal\n");
        printf("2. Member Portal\n");
        printf("3. Exit & Save\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("\n[Error] Invalid input! Please enter a number.\n");
            while (getchar() != '\n'); // Clear buffer
            continue;
        }

        switch (choice) {
            case 1:
                if (adminLogin()) {
                    adminDashboard(books, &bookCount, users, &userCount, transactions, &transCount);
                }
                break;
            case 2: {
                int loggedID = -1;
                if (authenticateUser(users, userCount, &loggedID)) {
                    memberDashboard(books, bookCount, users, userCount, transactions, &transCount);
                }
                break;
            }
            case 3:
                saveAllData(books, bookCount, users, userCount, transactions, transCount);
                printf("\n[Info] Data saved successfully. Exiting program. Goodbye!\n");
                break;
            default:
                printf("\n[Error] Invalid choice! Please enter 1, 2, or 3.\n");
        }
    } while (choice != 3);

    return 0;
}

// Admin Control Panel
void adminDashboard(Book books[], int *bookCount, User users[], int *userCount, Transaction transactions[], int *transCount) {
    int choice;
    do {
        printf("\n=========================================\n");
        printf("             ADMIN DASHBOARD             \n");
        printf("=========================================\n");
        printf("1. Add New Book\n");
        printf("2. View All Books\n");
        printf("3. Search Book\n");
        printf("4. Register New Member\n");
        printf("5. View Registered Members\n");
        printf("6. View Active Loans\n");
        printf("7. Issue Book to Member\n");
        printf("8. Process Book Return & Fines\n");
        printf("9. Logout\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("\n[Error] Invalid input!\n");
            while (getchar() != '\n');
            continue;
        }

        switch (choice) {
            case 1:
                addBook(books, bookCount);
                saveAllData(books, *bookCount, users, *userCount, transactions, *transCount);
                break;
            case 2:
                displayBooks(books, *bookCount);
                break;
            case 3:
                searchBook(books, *bookCount);
                break;
            case 4:
                registerMember(users, userCount);
                saveAllData(books, *bookCount, users, *userCount, transactions, *transCount);
                break;
            case 5:
                viewMemberList(users, *userCount);
                break;
            case 6:
                viewActiveLoans(transactions, *transCount);
                break;
            case 7:
                issueBook(books, *bookCount, users, *userCount, transactions, transCount);
                break;
            case 8:
                returnBook(books, *bookCount, transactions, *transCount);
                break;
            case 9:
                printf("\n[Info] Logging out from Admin Portal...\n");
                break;
            default:
                printf("\n[Error] Invalid choice! Choose between 1 and 9.\n");
        }
    } while (choice != 9);
}

// Member Portal
void memberDashboard(Book books[], int bookCount, User users[], int userCount, Transaction transactions[], int *transCount) {
    int choice;
    do {
        printf("\n=========================================\n");
        printf("             MEMBER PORTAL               \n");
        printf("=========================================\n");
        printf("1. View All Books\n");
        printf("2. Search Book\n");
        printf("3. Logout\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("\n[Error] Invalid input!\n");
            while (getchar() != '\n');
            continue;
        }

        switch (choice) {
            case 1:
                displayBooks(books, bookCount);
                break;
            case 2:
                searchBook(books, bookCount);
                break;
            case 3:
                printf("\n[Info] Logging out from Member Portal...\n");
                break;
            default:
                printf("\n[Error] Invalid choice! Choose between 1 and 3.\n");
        }
    } while (choice != 3);
}