#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

/* Simple file-based library system. All records are stored locally in .dat files. */
#define MEMBERS_FILE "members.dat"
#define BOOKS_FILE "books.dat"
#define LOANS_FILE "loans.dat"
#define LOG_FILE "activity.dat"
#define MAX_NAME 80
#define MAX_TEXT 100
#define LOAN_DAYS 14
#define FINE_PER_DAY 5

typedef struct { char id[8], name[MAX_NAME], mobile[20], password[80], created[24]; } Member;
typedef struct { int id, quantity; char title[MAX_TEXT], author[MAX_TEXT]; } Book;
typedef struct { int transactionId, bookId, fine; char memberId[8], issued[24], due[24], returned[24]; } Loan;
typedef struct { char memberId[8], action[32], when[24]; } Activity;

static void line(const char *prompt, char *out, size_t size) {
    size_t n; int ch;
    printf("%s", prompt);
    if (!fgets(out, (int)size, stdin)) { out[0] = '\0'; return; }
    n = strlen(out);
    if (n && out[n-1] == '\n') out[n-1] = '\0';
    else while ((ch = getchar()) != '\n' && ch != EOF) { }
}
static int number(const char *prompt) {
    char input[80];
    int value;
    char extra;

    while (1) {
        line(prompt, input, sizeof(input));
        if (sscanf(input, "%d %c", &value, &extra) == 1) {
            return value;
        }
        puts("Enter a valid number.");
    }
}

static void nowText(char out[24]) {
    time_t currentTime = time(NULL);
    struct tm *timeInfo = localtime(&currentTime);

    if (timeInfo != NULL) {
        strftime(out, 24, "%Y-%m-%d %H:%M:%S", timeInfo);
    } else {
        strcpy(out, "unknown");
    }
}

static void logAction(const char *id, const char *action) {
    FILE *file = fopen(LOG_FILE, "ab");
    Activity record;

    if (file == NULL) return;

    snprintf(record.memberId, sizeof(record.memberId), "%s", id ? id : "-");
    snprintf(record.action, sizeof(record.action), "%s", action);
    nowText(record.when);
    fwrite(&record, sizeof(record), 1, file);
    fclose(file);
}

static int mobileValid(const char *mobile) {
    size_t i;
    size_t length = strlen(mobile);

    if (length < 10 || length > 15) return 0;

    for (i = 0; i < length; i++) {
        if (!isdigit((unsigned char)mobile[i]) && !(i == 0 && mobile[i] == '+')) {
            return 0;
        }
    }
    return 1;
}

static int memberExists(const char *id) {
    FILE *file = fopen(MEMBERS_FILE, "rb");
    Member member;

    if (file == NULL) return 0;
    while (fread(&member, sizeof(member), 1, file) == 1) {
        if (strcmp(member.id, id) == 0) {
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    return 0;
}

static int mobileExists(const char *mobile) {
    FILE *file = fopen(MEMBERS_FILE, "rb");
    Member member;

    if (file == NULL) return 0;
    while (fread(&member, sizeof(member), 1, file) == 1) {
        if (strcmp(member.mobile, mobile) == 0) {
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    return 0;
}

static void makeId(char out[8]) {
    int tries = 0;

    do {
        snprintf(out, 8, "LIB%04u", (unsigned int)rand() % 10000u);
        tries++;
    } while (memberExists(out) && tries < 20000);

    if (memberExists(out)) strcpy(out, "LIB0000");
}

static int findBook(int id, Book *out) {
    FILE *file = fopen(BOOKS_FILE, "rb");
    Book book;

    if (file == NULL) return 0;
    while (fread(&book, sizeof(book), 1, file) == 1) {
        if (book.id == id) {
            if (out != NULL) *out = book;
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    return 0;
}

static int updateBook(Book wanted) {
    FILE *file = fopen(BOOKS_FILE, "r+b");
    Book book;

    if (file == NULL) return 0;
    while (fread(&book, sizeof(book), 1, file) == 1) {
        if (book.id == wanted.id) {
            fseek(file, -(long)sizeof(book), SEEK_CUR);
            fwrite(&wanted, sizeof(wanted), 1, file);
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    return 0;
}
static void registerMember(void)
{
    Member m;
    FILE *f;
    char otpText[20];
    char password[80];
    int otp, tries = 0;

    puts("\nNEW MEMBER REGISTRATION");
    line("Full name: ", m.name, sizeof(m.name));
    if (m.name[0] == '\0') { puts("Name cannot be empty."); return; }

    line("Mobile number (10-15 digits): ", m.mobile, sizeof(m.mobile));
    if (!mobileValid(m.mobile)) { puts("Invalid mobile number."); return; }
    if (mobileExists(m.mobile)) { puts("This mobile number is already registered."); return; }

    otp = 1000 + rand() % 9000;
    printf("\nDEMO OTP for %s: %04d\n", m.mobile, otp);
    puts("Demo only: no SMS is sent. A real SMS gateway is required for mobile delivery.");

    while (tries < 3)
    {
        line("Enter 4-digit OTP: ", otpText, sizeof(otpText));
        if (strlen(otpText) == 4 && strspn(otpText, "0123456789") == 4 && atoi(otpText) == otp) break;
        tries++;
        printf("Incorrect OTP. Attempts left: %d\n", 3 - tries);
    }
    if (tries == 3) { puts("Verification failed."); return; }

    line("Create password (minimum 4 characters): ", password, sizeof(password));
    if (strlen(password) < 4) { puts("Password too short."); return; }
    strcpy(m.password, password);
    nowText(m.created);
    makeId(m.id);
    if (memberExists(m.id)) { puts("Could not allocate a unique ID; try again."); return; }

    f = fopen(MEMBERS_FILE, "ab");
    if (f == NULL) { puts("Could not save member record."); return; }
    if (fwrite(&m, sizeof(m), 1, f) != 1)
    {
        puts("Could not save member record.");
        fclose(f);
        return;
    }
    fclose(f);
    printf("Registration successful. Your Library ID is %s\n", m.id);
    puts("Keep this ID for future login.");
    logAction(m.id, "MEMBER_REGISTERED");
}

static int memberLogin(char id[8])
{
    char password[80];
    Member m;
    FILE *f;

    line("Library ID (LIBXXXX): ", id, 8);
    line("Password: ", password, sizeof(password));
    f = fopen(MEMBERS_FILE, "rb");
    if (f == NULL) { puts("No members registered yet."); return 0; }

    while (fread(&m, sizeof(m), 1, f) == 1)
    {
        if (strcmp(m.id, id) == 0 && strcmp(m.password, password) == 0)
        {
            printf("Welcome, %s.\n", m.name);
            fclose(f);
            logAction(id, "MEMBER_LOGIN");
            return 1;
        }
    }
    fclose(f);
    puts("Incorrect Library ID or password.");
    return 0;
}

static void addBook(void)
{
    Book b, old;
    FILE *f;

    b.id = number("Book ID: ");
    if (findBook(b.id, &old)) { puts("That book ID already exists."); return; }
    b.quantity = number("Quantity: ");
    if (b.quantity < 0) { puts("Quantity cannot be negative."); return; }
    line("Title: ", b.title, sizeof(b.title));
    line("Author: ", b.author, sizeof(b.author));

    f = fopen(BOOKS_FILE, "ab");
    if (f == NULL) { puts("Could not open books file."); return; }
    fwrite(&b, sizeof(b), 1, f);
    fclose(f);
    puts("Book added.");
}

static void viewBooks(void)
{
    FILE *f;
    Book b;
    int found = 0;
    const char *border = "+------+--------------------------------+------------------------------+-----------+";

    f = fopen(BOOKS_FILE, "rb");
    if (f == NULL) { puts("No books found."); return; }

    puts("\n                              LIBRARY BOOK CATALOG");
    puts(border);
    printf("| %-4s | %-30s | %-28s | %-9s |\n", "ID", "TITLE", "AUTHOR", "AVAILABLE");
    puts(border);

    while (fread(&b, sizeof(b), 1, f) == 1)
    {
        printf("| %-4d | %-30.30s | %-28.28s | %9d |\n", b.id, b.title, b.author, b.quantity);
        found = 1;
    }
    if (found == 0) puts("|                  No books have been added yet.                         |");
    puts(border);
    fclose(f);
}

static void viewMembers(void)
{
    FILE *f;
    Member m;
    f = fopen(MEMBERS_FILE, "rb");
    if (f == NULL) { puts("No members registered."); return; }
    puts("\nLIBRARY ID\tNAME\tMOBILE\tCREATED");
    while (fread(&m, sizeof(m), 1, f) == 1)
        printf("%s\t%s\t%s\t%s\n", m.id, m.name, m.mobile, m.created);
    fclose(f);
}

static void issueBook(const char *memberId)
{
    int id;
    Book b;
    Loan l;
    FILE *f;
    time_t dueTime;
    struct tm *timeInfo;

    id = number("Book ID to issue: ");
    if (!findBook(id, &b) || b.quantity < 1) { puts("Book not found or no copies available."); return; }
    b.quantity--;
    if (!updateBook(b)) { puts("Could not update stock."); return; }

    memset(&l, 0, sizeof(l));
    l.bookId = id;
    strcpy(l.memberId, memberId);
    nowText(l.issued);
    dueTime = time(NULL) + (time_t)LOAN_DAYS * 86400;
    timeInfo = localtime(&dueTime);
    if (timeInfo != NULL) strftime(l.due, sizeof(l.due), "%Y-%m-%d %H:%M:%S", timeInfo);

    f = fopen(LOANS_FILE, "ab");
    if (f == NULL)
    {
        b.quantity++;
        updateBook(b);
        puts("Could not save loan.");
        return;
    }
    fseek(f, 0, SEEK_END);
    l.transactionId = (int)(ftell(f) / (long)sizeof(l)) + 1;
    fwrite(&l, sizeof(l), 1, f);
    fclose(f);
    printf("Book issued. Transaction %d; due %s\n", l.transactionId, l.due);
    logAction(memberId, "BOOK_ISSUED");
}

static void returnBook(const char *memberId)
{
    int id, fine = 0, found = 0;
    FILE *f;
    Loan l;
    Book b;
    time_t currentTime, dueTime;
    struct tm dueDate;

    id = number("Book ID to return: ");
    f = fopen(LOANS_FILE, "r+b");
    if (f == NULL) { puts("No loan records."); return; }

    while (fread(&l, sizeof(l), 1, f) == 1)
    {
        if (l.bookId == id && strcmp(l.memberId, memberId) == 0 && l.returned[0] == '\0')
        {
            found = 1;
            break;
        }
    }
    if (found == 0) { fclose(f); puts("No active loan found."); return; }

    memset(&dueDate, 0, sizeof(dueDate));
    if (sscanf(l.due, "%d-%d-%d %d:%d:%d", &dueDate.tm_year, &dueDate.tm_mon,
               &dueDate.tm_mday, &dueDate.tm_hour, &dueDate.tm_min, &dueDate.tm_sec) == 6)
    {
        dueDate.tm_year -= 1900;
        dueDate.tm_mon--;
        dueDate.tm_isdst = -1;
        dueTime = mktime(&dueDate);
        currentTime = time(NULL);
        if (currentTime > dueTime)
            fine = ((int)(difftime(currentTime, dueTime) / 86400) + 1) * FINE_PER_DAY;
    }

    nowText(l.returned);
    l.fine = fine;
    fseek(f, -(long)sizeof(l), SEEK_CUR);
    fwrite(&l, sizeof(l), 1, f);
    fclose(f);

    if (findBook(id, &b)) { b.quantity++; updateBook(b); }
    printf("Book returned. Fine: Rs. %d\n", fine);
    logAction(memberId, "BOOK_RETURNED");
}

static void viewLoans(const char *filter)
{
    FILE *f;
    Loan l;
    f = fopen(LOANS_FILE, "rb");
    if (f == NULL) { puts("No transactions recorded."); return; }
    puts("\nTRANSACTIONS\nTXN\tLIBRARY ID\tBOOK\tISSUED\tDUE\tRETURNED\tFINE");
    while (fread(&l, sizeof(l), 1, f) == 1)
    {
        if (filter == NULL || strcmp(filter, l.memberId) == 0)
        {
            printf("%d\t%s\t%d\t%s\t%s\t%s\t%d\n", l.transactionId, l.memberId,
                   l.bookId, l.issued, l.due,
                   l.returned[0] ? l.returned : "Not returned", l.fine);
        }
    }
    fclose(f);
}

static void viewActivity(void)
{
    FILE *f;
    Activity a;
    f = fopen(LOG_FILE, "rb");
    if (f == NULL) { puts("No activity recorded."); return; }
    puts("\nACTIVITY LOG\nLIBRARY ID\tACTION\tTIME");
    while (fread(&a, sizeof(a), 1, f) == 1)
        printf("%s\t%s\t%s\n", a.memberId, a.action, a.when);
    fclose(f);
}

static int adminLogin(void)
{
    char username[60], password[60];
    line("Admin username: ", username, sizeof(username));
    line("Admin password: ", password, sizeof(password));
    if (strcmp(username, "ansh") == 0 && strcmp(password, "Library@2026") == 0)
    {
        puts("Admin login successful. Welcome, Ansh!");
        return 1;
    }
    puts("Incorrect username or password.");
    return 0;
}

static void adminMenu(void)
{
    int choice;
    char id[8];
    do
    {
        puts("\nADMIN MENU\n1. Add book\n2. View books\n3. View members\n4. Issue book to member\n5. Return a book for member\n6. View all transactions\n7. View activity log\n8. Log out");
        choice = number("Choice: ");
        switch (choice)
        {
            case 1: addBook(); break;
            case 2: viewBooks(); break;
            case 3: viewMembers(); break;
            case 4:
                line("Member Library ID: ", id, sizeof(id));
                if (memberExists(id)) issueBook(id); else puts("Member not found.");
                break;
            case 5:
                line("Member Library ID: ", id, sizeof(id));
                if (memberExists(id)) returnBook(id); else puts("Member not found.");
                break;
            case 6: viewLoans(NULL); break;
            case 7: viewActivity(); break;
            case 8: puts("Logged out."); break;
            default: puts("Choose 1-8.");
        }
    } while (choice != 8);
}

static void memberMenu(const char *id)
{
    int choice;
    do
    {
        puts("\nMEMBER MENU\n1. View books\n2. Issue a book\n3. Return a book\n4. View my borrowing history\n5. Log out");
        choice = number("Choice: ");
        switch (choice)
        {
            case 1: viewBooks(); break;
            case 2: issueBook(id); break;
            case 3: returnBook(id); break;
            case 4: viewLoans(id); break;
            case 5: puts("Logged out."); break;
            default: puts("Choose 1-5.");
        }
    } while (choice != 5);
}

int main(void)
{
    int choice;
    char id[8];
    srand((unsigned int)time(NULL));
    do
    {
        puts("\nLIBRARY MANAGEMENT SYSTEM\nDefault administrator: ansh\n1. Admin login\n2. Member login\n3. New member registration\n4. Exit");
        choice = number("Choice: ");
        switch (choice)
        {
            case 1: if (adminLogin()) adminMenu(); break;
            case 2: if (memberLogin(id)) memberMenu(id); break;
            case 3: registerMember(); break;
            case 4: puts("Records are saved in members.dat, books.dat, loans.dat and activity.dat. Goodbye."); break;
            default: puts("Choose 1-4.");
        }
    } while (choice != 4);
    return 0;
}
