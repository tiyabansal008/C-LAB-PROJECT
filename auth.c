#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "auth.h"

// Hardcoded Master Admin Credentials
#define ADMIN_USER "admin"
#define ADMIN_PASS "admin123"

// Function to verify admin credentials
int adminLogin() {
    char username[30];
    char password[30];

    printf("\n==============================\n");
    printf("         ADMIN LOGIN          \n");
    printf("==============================\n");
    printf("Enter Admin Username: ");
    
    // Clear input buffer if leftover newline exists
    while (getchar() != '\n' && feof(stdin) == 0); // safe clearing if needed, or simple getchar()
    
    if (fgets(username, sizeof(username), stdin) != NULL) {
        username[strcspn(username, "\n")] = 0;
    }

    printf("Enter Admin Password: ");
    if (fgets(password, sizeof(password), stdin) != NULL) {
        password[strcspn(password, "\n")] = 0;
    }

    if (strcmp(username, ADMIN_USER) == 0 && strcmp(password, ADMIN_PASS) == 0) {
        printf("\n[Success] Admin Login Successful!\n");
        return 1; // Access Granted
    } else {
        printf("\n[Error] Access Denied! Invalid username or password.\n");
        return 0; // Access Denied
    }
}

// Function to register a new library member
void registerMember(User users[], int *userCount) {
    if (*userCount >= MAX_USERS) {
        printf("\n[Error] Member registration limit reached!\n");
        return;
    }

    printf("\n==============================\n");
    printf("     REGISTER NEW MEMBER      \n");
    printf("==============================\n");
    printf("Enter Member ID (Numeric): ");
    scanf("%d", &users[*userCount].id);
    getchar(); // Clear buffer newline

    printf("Enter Member Name: ");
    if (fgets(users[*userCount].name, sizeof(users[*userCount].name), stdin) != NULL) {
        users[*userCount].name[strcspn(users[*userCount].name, "\n")] = 0;
    }

    printf("Enter Password: ");
    if (fgets(users[*userCount].password, sizeof(users[*userCount].password), stdin) != NULL) {
        users[*userCount].password[strcspn(users[*userCount].password, "\n")] = 0;
    }

    (*userCount)++;
    printf("\n[Success] Member registered successfully!\n");
    saveUsersToFile(users, *userCount);
}

// Function to authenticate a member login
int authenticateUser(User users[], int userCount, int *loggedID) {
    int id;
    char password[20];

    printf("\n==============================\n");
    printf("         MEMBER LOGIN         \n");
    printf("==============================\n");
    printf("Enter Member ID: ");
    scanf("%d", &id);
    getchar(); // Clear buffer newline

    printf("Enter Password: ");
    if (fgets(password, sizeof(password), stdin) != NULL) {
        password[strcspn(password, "\n")] = 0;
    }

    for (int i = 0; i < userCount; i++) {
        if (users[i].id == id && strcmp(users[i].password, password) == 0) {
            *loggedID = users[i].id;
            printf("\n[Success] Welcome back, %s!\n", users[i].name);
            return 1; // Login Success
        }
    }

    printf("\n[Error] Invalid Member ID or Password!\n");
    return 0; // Login Failed
}

// Function to display all registered members (Admin view)
void viewMemberList(User users[], int userCount) {
    if (userCount == 0) {
        printf("\n[Info] No registered members found in the system.\n");
        return;
    }

    printf("\n=========================================\n");
    printf("         REGISTERED MEMBERS LIST         \n");
    printf("=========================================\n");
    printf("%-10s | %-30s\n", "ID", "Name");
    printf("-----------------------------------------\n");
    for (int i = 0; i < userCount; i++) {
        printf("%-10d | %-30s\n", users[i].id, users[i].name);
    }
    printf("=========================================\n");
}

// Function to save user records to binary file
void saveUsersToFile(User users[], int userCount) {
    FILE *file = fopen(USER_FILE, "wb");
    if (file == NULL) {
        return;
    }
    fwrite(&userCount, sizeof(int), 1, file);
    fwrite(users, sizeof(User), userCount, file);
    fclose(file);
}

// Function to load user records from binary file on startup
int loadUsersFromFile(User users[]) {
    FILE *file = fopen(USER_FILE, "rb");
    if (file == NULL) {
        return 0; // No file exists yet
    }
    int count = 0;
    fread(&count, sizeof(int), 1, file);
    fread(users, sizeof(User), count, file);
    fclose(file);
    return count;
}