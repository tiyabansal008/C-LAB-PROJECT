#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "book.h"

// Function to add a new book to the inventory
void addBook(Book books[], int *count) {
    if (*count >= MAX_BOOKS) {
        printf("\n[Error] Library is full. Cannot add more books.\n");
        return;
    }

    printf("\n==============================\n");
    printf("         ADD NEW BOOK         \n");
    printf("==============================\n");
    printf("Enter Book ID: ");
    scanf("%d", &books[*count].id);
    getchar(); // Clear leftover newline from input buffer

    printf("Enter Book Title: ");
    if (fgets(books[*count].title, sizeof(books[*count].title), stdin) != NULL) {
        books[*count].title[strcspn(books[*count].title, "\n")] = 0; // Remove newline
    }

    printf("Enter Author Name: ");
    if (fgets(books[*count].author, sizeof(books[*count].author), stdin) != NULL) {
        books[*count].author[strcspn(books[*count].author, "\n")] = 0; // Remove newline
    }

    printf("Enter Quantity: ");
    scanf("%d", &books[*count].quantity);

    (*count)++;
    printf("\n[Success] Book added successfully!\n");
    saveBooksToFile(books, *count);
}

// Function to display all books in a neat table
void displayBooks(Book books[], int count) {
    if (count == 0) {
        printf("\n[Info] No books available in the library.\n");
        return;
    }

    printf("\n=================================================================\n");
    printf("                         LIBRARY CATALOG                         \n");
    printf("=================================================================\n");
    printf("%-6s | %-30s | %-20s | %-8s\n", "ID", "Title", "Author", "Qty");
    printf("-----------------------------------------------------------------\n");
    
    for (int i = 0; i < count; i++) {
        printf("%-6d | %-30s | %-20s | %-8d\n", 
               books[i].id, books[i].title, books[i].author, books[i].quantity);
    }
    printf("=================================================================\n");
}

// Function to search for a book by its title
void searchBook(Book books[], int count) {
    if (count == 0) {
        printf("\n[Info] The library catalog is empty.\n");
        return;
    }

    char query[50];
    printf("\nEnter book title to search: ");
    getchar(); // Clear buffer
    if (fgets(query, sizeof(query), stdin) != NULL) {
        query[strcspn(query, "\n")] = 0;
    }

    int found = 0;
    printf("\n--- Search Results ---\n");
    for (int i = 0; i < count; i++) {
        // Simple case-insensitive comparison using standard library or exact match
        if (_stricmp(books[i].title, query) == 0) {
            printf("ID: %d | Title: %s | Author: %s | Qty: %d\n", 
                   books[i].id, books[i].title, books[i].author, books[i].quantity);
            found = 1;
        }
    }

    if (!found) {
        printf("No book found matching \"%s\".\n", query);
    }
}

// Function to save book inventory to a binary file
void saveBooksToFile(Book books[], int count) {
    FILE *file = fopen(BOOK_FILE, "wb");
    if (file == NULL) {
        return;
    }
    fwrite(&count, sizeof(int), 1, file);
    fwrite(books, sizeof(Book), count, file);
    fclose(file);
}

// Function to load book inventory from a binary file on startup
int loadBooksFromFile(Book books[]) {
    FILE *file = fopen(BOOK_FILE, "rb");
    if (file == NULL) {
        return 0; // File does not exist yet
    }
    int count = 0;
    fread(&count, sizeof(int), 1, file);
    fread(books, sizeof(Book), count, file);
    fclose(file);
    return count;
}