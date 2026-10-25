#ifndef AUTH_H
#define AUTH_H

#define MAX_USERS 100
#define USER_FILE "users_data.dat"

// Structure to represent a Library Member
typedef struct {
    int id;
    char name[50];
    char password[20];
} User;

// Function Prototypes
int adminLogin();
void registerMember(User users[], int *userCount);
int authenticateUser(User users[], int userCount, int *loggedID);
void viewMemberList(User users[], int userCount);
void saveUsersToFile(User users[], int userCount);
int loadUsersFromFile(User users[]);

#endif