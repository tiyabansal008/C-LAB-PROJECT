#ifndef BOOK_H
#define BOOK_H

#define MAX_BOOKS 100
#define BOOK_FILE "books_data.dat"

// Simple structure for a Book
typedef struct {
    int id;
    char title[50];
    char author[50];
    int quantity;
} Book;

// Function Prototypes
void addBook(Book books[], int *count);
void displayBooks(Book books[], int count);
void searchBook(Book books[], int count);
void saveBooksToFile(Book books[], int count);
int loadBooksFromFile(Book books[]);

#endif