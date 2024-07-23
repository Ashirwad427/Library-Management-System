#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include "data.h"

int main() {
    char filename[] = "database.dat";
    FILE* data_fp = fopen(filename, "wb+");
    int rec_count = 5;

    // Creating 5 struct objects
    struct data book1, book2, book3, book4, book5;
    int rec_size = sizeof(book1);

    // Initialize isdeleted to zero for each book
    book1.isdeleted = 0;
    book2.isdeleted = 0;
    book3.isdeleted = 0;
    book4.isdeleted = 0;
    book5.isdeleted = 0;

    // Assigning values to attributes of each struct object
    book1.BookID = 1;
    strcpy(book1.BookTitle, "The Great Gatsby");
    strcpy(book1.AuthorName, "F. Scott Fitzgerald");
    strcpy(book1.Genre, "Fiction");
    book1.Qty = 10;

    book2.BookID = 2;
    strcpy(book2.BookTitle, "To Kill a Mockingbird");
    strcpy(book2.AuthorName, "Harper Lee");
    strcpy(book2.Genre, "Fiction");
    book2.Qty = 8;

    book3.BookID = 3;
    strcpy(book3.BookTitle, "1984");
    strcpy(book3.AuthorName, "George Orwell");
    strcpy(book3.Genre, "Dystopian Fiction");
    book3.Qty = 12;

    book4.BookID = 4;
    strcpy(book4.BookTitle, "Pride and Prejudice");
    strcpy(book4.AuthorName, "Jane Austen");
    strcpy(book4.Genre, "Romance");
    book4.Qty = 6;

    book5.BookID = 5;
    strcpy(book5.BookTitle, "The Catcher in the Rye");
    strcpy(book5.AuthorName, "J.D. Salinger");
    strcpy(book5.Genre, "New-age Fiction");
    book5.Qty = 15;

    fseek(data_fp, 0, SEEK_SET);

    fwrite(&rec_count, sizeof(int), 1, data_fp);

    // Write each book record along with BookID
    fwrite(&book1.BookID, sizeof(int), 1, data_fp);
    fwrite(&book1, rec_size, 1, data_fp);

    fwrite(&book2.BookID, sizeof(int), 1, data_fp);
    fwrite(&book2, rec_size, 1, data_fp);

    fwrite(&book3.BookID, sizeof(int), 1, data_fp);
    fwrite(&book3, rec_size, 1, data_fp);

    fwrite(&book4.BookID, sizeof(int), 1, data_fp);
    fwrite(&book4, rec_size, 1, data_fp);

    fwrite(&book5.BookID, sizeof(int), 1, data_fp);
    fwrite(&book5, rec_size, 1, data_fp);

    fclose(data_fp);
}
