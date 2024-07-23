#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/file.h>
#include <pthread.h>
#include "user.h"
#include "data.h"

#define MAX_MSG_SIZE 1024
#define MAXIMUM_BOOK_COUNT 20
#define MAX_CLIENTS 10
#define BUFFER_SIZE 256
#define DATABASE_FILE "database.dat"
#define AUTHENTICATION_FAILED 404
#define AUTHENTICATION_SUCCESS 505

void error(const char * msg){
    perror(msg);
    exit(1);
}

void *handleClient(void *arg);
int userLogin(int client_fd);
int adminLogin(int client_fd);
void readDatabase(char * buffer,int newsockfd);
void borrowBook(int newsockfd);
void returnBook(int newsockfd);
void readBookData(int newsockfd);
void addBook(int newsockfd);
void deleteBook(int newsockfd);
void modifyBook(int newsockfd);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int portno = atoi(argv[1]);
    int sockfd, newsockfd;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error("Error opening socket");
    }

    // Initialize socket structure
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    // Bind the socket
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("Error on binding");
    }

    // Listen for connections
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    while (1) {
        // Accept connection from client
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) {
            error("Error on accept");
        }

        // Create a new thread to handle the client
        pthread_t tid;
        if (pthread_create(&tid, NULL, handleClient, (void *)&newsockfd) != 0) {
            perror("Error creating thread");
            close(newsockfd);
        }
    }

    close(sockfd);
    return 0;
}

//Rest of your functions...

void *handleClient(void *arg) {
    int newsockfd = *((int *)arg);
    char buffer[BUFFER_SIZE];
    bzero(buffer, BUFFER_SIZE);

    char choice;
    read(newsockfd, &choice, sizeof(char));

    if (choice == '1') { // User Login
        if (userLogin(newsockfd) == AUTHENTICATION_FAILED) {
            char failure_msg[] = "Authentication failed\n";
            send(newsockfd, failure_msg, sizeof(failure_msg), 0);
            pthread_exit(NULL);
        } else {
            char success_msg[] = "User Login successful\n";
            send(newsockfd, success_msg, sizeof(success_msg), 0);
            // Handle user requests...
        }
    } else if (choice == '2') { // Member Login
        if (adminLogin(newsockfd) == AUTHENTICATION_FAILED) {
            char failure_msg[] = "Authentication failed\n";
            send(newsockfd, failure_msg, sizeof(failure_msg), 0);
            pthread_exit(NULL);
        } else {
            char success_msg[] = " Administrator Login successful\n";
            send(newsockfd, success_msg, sizeof(success_msg), 0);
            // Handle admin requests...
        }
    }
    if(choice=='3'){
        printf("Returned before authentication.\n");
        pthread_exit(NULL);
    }
    if(choice=='1'){                //User options

    while(1){
        char option;
        read(newsockfd,&option,sizeof(option));

        if(option=='4'){
            close(newsockfd);
            printf("\nUser logged out.\n");
            pthread_exit(NULL);
        }
        else if(option=='1'){
            readDatabase(buffer,newsockfd);
        }
        else if(option=='2'){
            borrowBook(newsockfd);
        }
        else if(option=='3'){
            returnBook(newsockfd);
        }
    }
    }
    else if(choice=='2'){           //Admin options
    while(1){
        char option;
        read(newsockfd,&option,sizeof(option));

        if(option=='7'){
            close(newsockfd);
            printf("\nAdmin logged out.\n");
            pthread_exit(NULL);
        }
        else if(option=='1'){
            readBookData(newsockfd);
        }
        else if(option=='2'){
            addBook(newsockfd);
        }
        else if(option=='3'){
            deleteBook(newsockfd);
        }
        else if(option=='4'){
            modifyBook(newsockfd);
        }
        else if(option=='5'){
            //directly file read by client
        }
        else if(option=='6'){
            readDatabase(buffer,newsockfd);
        }
    }
    }
}

int userLogin(int client_fd) {
    struct credential client;

    read(client_fd, client.username, sizeof(client.username)-1);
    read(client_fd, client.password, sizeof(client.password)-1);

    FILE *read_user_fp = fopen("users.dat", "rb+");
    if (read_user_fp == NULL) {
        error("Error opening users.dat");
    }

    fseek(read_user_fp,0,SEEK_SET);
    int n; // Number of users
    fread(&n, sizeof(int), 1, read_user_fp);
    struct credential temp;

    for (int i = 0; i < n; i++) {
        fread(&temp, sizeof(temp), 1, read_user_fp);
        if (strcmp(client.username, temp.username) == 0) {
            break;
        }
    }

    fclose(read_user_fp);

    // printf("User input : %s \n",client.username);
    // printf("Expected : %s \n",temp.username);

    // printf("User input : %s \n",client.password);
    // printf("Expected : %s \n",temp.password);

    if (strcmp(client.username,temp.username)!=0 || strcmp(client.password, temp.password) != 0) {
        printf("AUTHENTICATION FAILED.\n");
        return AUTHENTICATION_FAILED;
    } else {
        printf("AUTHENTICATION SUCCESSFUL.\n");
        printf("%s Logged in successfully\n", client.username);
        return AUTHENTICATION_SUCCESS;
    }
}

int adminLogin(int client_fd) {
    struct credential client;

    read(client_fd, client.username, sizeof(client.username));
    read(client_fd, client.password, sizeof(client.password));

    FILE *read_admin_fp = fopen("admins.dat", "rb+");
    if (read_admin_fp == NULL) {
        error("Error opening admins.dat");
    }

    fseek(read_admin_fp,0,SEEK_SET);
    int n; // Number of admins
    fread(&n, sizeof(int), 1, read_admin_fp);
    struct credential temp;

    for (int i = 0; i < n; i++) {
        fread(&temp, sizeof(temp), 1, read_admin_fp);
        if (strcmp(client.username, temp.username) == 0) {
            break;
        }
    }

    fclose(read_admin_fp);

    if (strcmp(client.username,temp.username)!=0 || strcmp(client.password, temp.password) != 0) {
        printf("AUTHENTICATION FAILED.\n");
        return AUTHENTICATION_FAILED;
    } else {
        printf("AUTHENTICATION SUCCESSFUL.\n");
        printf("%s Logged in successfully\n", client.username);
        return AUTHENTICATION_SUCCESS;
    }
}

void readDatabase(char *buffer, int newsockfd) {
    printf("\n USER OPTION 1/ADMIN OPTION 6 SELECTED : READ ALL RECORDS\n");
    int fd = open("database.dat", O_RDONLY);
    if (fd < 0) {
        perror("Error opening database");
        strcpy(buffer, "Error reading database"); // Send error to client
        send(newsockfd,buffer,BUFFER_SIZE,0);
        return;
    }

    flock(fd, LOCK_SH); // Shared lock for reading

    lseek(fd, 0, SEEK_SET);
    //sleep(20);
    int number_of_records;
    read(fd, &number_of_records, sizeof(int));

    send(newsockfd,&number_of_records,sizeof(int),0);

    //printf("Number of records : %d\n", number_of_records);

    // Allocate memory for records
    struct data *records = malloc(number_of_records * sizeof(struct data));
    if (records == NULL) {
        perror("Memory allocation failed");
        strcpy(buffer, "Error reading database"); // Send error to client
        flock(fd, LOCK_UN); // Unlock
        close(fd);
        return;
    }

    // Read records from file
    for (int i = 0; i < number_of_records; i++) {
        int bookID;
        read(fd, &bookID, sizeof(int));
        if (read(fd, &records[i], sizeof(struct data)) != sizeof(struct data)) {
            fprintf(stderr, "Error reading record %d\n", i);
            free(records);
            strcpy(buffer, "Error reading database"); // Send error to client
            flock(fd, LOCK_UN); // Unlock
            close(fd);
            return;
        }
        //printf("%d. %d %s %s %d %s \n", i, bookID, records[i].AuthorName, records[i].BookTitle, records[i].Qty, records[i].Genre);
    }

    // Sending records to the client process
    int bytes_sent = send(newsockfd, records, number_of_records * sizeof(struct data), 0);
    if (bytes_sent == -1) {
        perror("Error sending data to client");
        strcpy(buffer, "Error sending data to client");
    } else {
        printf("Sent %d bytes of data to client\n", bytes_sent);
    }

    free(records);

    flock(fd, LOCK_UN); // Unlock
    close(fd);

    // Free dynamically allocated memory
}

// Function to handle borrowBook operation
void borrowBook(int newsockfd) {
    printf("\n USER OPTION 2 SELECTED : BORROW A BOOK\n");
    int bookID;
    read(newsockfd, &bookID, sizeof(int));

    int fd = open(DATABASE_FILE, O_RDWR);
    if (fd < 0) {
        perror("Error opening database");
        return;
    }


    //printf("\n%ld thread trying to acquire lock\n",pthread_self());
    flock(fd, LOCK_EX); // Exclusive lock for writing
    //printf("\n%ld thread has acquired the lock\n",pthread_self());


    // Move file pointer to beginning to start reading records
    lseek(fd, 0, SEEK_SET);

    int number_of_records;
    read(fd, &number_of_records, sizeof(int));

    //printf("Number of records read - %d\n",number_of_records);

    // Search for the book with the given BookID
    struct data bookData;
    int bid;
    int found = 0;
    for (int i = 0; i < number_of_records; i++) {
        read(fd,&bid,sizeof(int));
        if (read(fd, &bookData, sizeof(struct data)) == -1) {
            perror("Error reading database");
            flock(fd, LOCK_UN); // Unlock
            close(fd);
            return;
        }
        if (bookData.BookID == bookID && bookData.isdeleted==0) {
            found = 1;
            printf("FOUND BOOK\n");
            break;
        }
    }
    //printf("Record found - %d %s %s %d %s\n",bookData.BookID,bookData.AuthorName,bookData.BookTitle,bookData.Qty,bookData.Genre);
    
    if (!found) {
        printf("Book with ID %d not found\n", bookID);
        // Send message to client indicating book not found
        char msg[] = "Book not found";
        send(newsockfd, msg, sizeof(msg), 0);
    } else {
        // Update quantity and write back to the database
        if (bookData.Qty > 0) {
            //sleep(15);
            bookData.Qty--;
            // Seek back to the beginning of the record
            lseek(fd, -(off_t)sizeof(struct data), SEEK_CUR);
            if (write(fd, &bookData, sizeof(struct data)) == -1) {
                perror("Error writing to database");
                flock(fd, LOCK_UN); // Unlock
                close(fd);
                return;
            }
            printf("Book with ID %d borrowed successfully\n", bookID);
            // Send success message to client
            char msg[] = "Book borrowed successfully";
            send(newsockfd, msg, sizeof(msg), 0);
        } else {
            printf("Book with ID %d is out of stock\n", bookID);
            // Send message to client indicating book out of stock
            char msg[] = "Book out of stock";
            send(newsockfd, msg, sizeof(msg), 0);
        }
    }

    flock(fd, LOCK_UN); // Unlock
    //printf("\n %ld thread has released the lock\n",pthread_self());
    close(fd);
}

// Function to handle returnBook operation
void returnBook(int newsockfd) {
    printf("\nUSER OPTION 3 SELECTED : RETURN A BOOK\n");
    int bookID;
    read(newsockfd, &bookID, sizeof(int));

    int fd = open(DATABASE_FILE, O_RDWR);
    if (fd < 0) {
        perror("Error opening database");
        return;
    }

    flock(fd, LOCK_EX); // Exclusive lock for writing

    // Move file pointer to beginning to start reading records
    lseek(fd, 0, SEEK_SET);

    int number_of_records;
    read(fd, &number_of_records, sizeof(int));

    //printf("Number of records read - %d\n", number_of_records);

    // Search for the book with the given BookID
    struct data bookData;
    int bid;
    int found = 0;
    for (int i = 0; i < number_of_records; i++) {
        read(fd, &bid, sizeof(int));
        if (read(fd, &bookData, sizeof(struct data)) == -1) {
            perror("Error reading database");
            flock(fd, LOCK_UN); // Unlock
            close(fd);
            return;
        }
        if (bookData.BookID == bookID && bookData.isdeleted==0) {
            found = 1;
            printf("FOUND BOOK\n");
            break;
        }
    }
    
    if (!found) {
        printf("Book with ID %d not found\n", bookID);
        // Send message to client indicating book not found
        char msg[] = "Book not found";
        send(newsockfd, msg, sizeof(msg), 0);
    } else {
        // Update quantity and write back to the database
        bookData.Qty++;
        // Seek back to the beginning of the record
        lseek(fd, -(off_t)sizeof(struct data), SEEK_CUR);
        if (write(fd, &bookData, sizeof(struct data)) == -1) {
            perror("Error writing to database");
            flock(fd, LOCK_UN); // Unlock
            close(fd);
            return;
        }
        printf("Book with ID %d returned successfully\n", bookID);
        // Send success message to client
        char msg[] = "Book returned successfully";
        send(newsockfd, msg, sizeof(msg), 0);
    }

    flock(fd, LOCK_UN); // Unlock
    close(fd);
}

void readBookData(int newsockfd) {
    printf("\nADMIN OPTION 1 SELECTED : READ A RECORD\n");
    int fd = open("database.dat", O_RDONLY);
    if (fd < 0) {
        perror("Error opening database");
        return;
    }

    flock(fd, LOCK_SH); // Shared lock for reading

    lseek(fd, 0, SEEK_SET);
    
    int number_of_records;
    read(fd, &number_of_records, sizeof(int));

    int bID;
    read(newsockfd, &bID, sizeof(int));

    struct data *record = malloc(sizeof(struct data));
    if (record == NULL) {
        perror("Memory allocation failed");
        flock(fd, LOCK_UN); // Unlock
        close(fd);
        return;
    }
    
    int found = 0;
    for (int i = 0; i < number_of_records; i++) {
        int bookID;
        read(fd, &bookID, sizeof(int));
        if (read(fd, record, sizeof(struct data)) != sizeof(struct data)) {
            fprintf(stderr, "Error reading record %d\n", i);
            free(record);
            flock(fd, LOCK_UN); // Unlock
            close(fd);
            return;
        }
        if (bID == bookID && record->isdeleted == 0) {
            found = 1;
            send(newsockfd, record, sizeof(struct data), 0);
            printf("FOUND BOOK\n");
            break;
        }
    }

    if (found == 0) {
        printf("Book with ID %d not found\n", bID);
        // Set record's author name to "Book not found"
        strcpy(record->AuthorName, "Book not found");
        send(newsockfd, record, sizeof(struct data), 0);
    }

    free(record);
    flock(fd, LOCK_UN); // Unlock
    close(fd);
}


void addBook(int newsockfd){
    printf("\nADMIN OPTION 2 SELECTED : ADD A BOOK TO DATABASE\n");
    int fd = open(DATABASE_FILE, O_RDWR);
    if (fd < 0) {
        perror("Error opening database");
        return;
    }
    struct data * newbook=malloc(REC_SIZE);
    read(newsockfd,newbook,REC_SIZE);


    flock(fd, LOCK_EX); // Exclusive lock for writing

    //sleep(20);            //for testing write lock
    // Move file pointer to beginning to start reading records
    lseek(fd, 0, SEEK_SET);

    int number_of_records;
    read(fd,&number_of_records,sizeof(int));

    
    // Search for the book with the given BookID
    struct data bookData;
    int bid;
    int found = 0;
    for (int i = 0; i < number_of_records; i++) {
        read(fd, &bid, sizeof(int));
        if (read(fd, &bookData, sizeof(struct data)) == -1) {
            perror("Error reading database");
            flock(fd, LOCK_UN); // Unlock
            close(fd);
            return;
        }
        if (bid == newbook->BookID && bookData.isdeleted==1) {
            found = 1;
            printf("THIS BOOK RECORD WAS PREVIOUSLY DELETED. REPLACING RECORD\n");
            break;
        }
        else if(bid == newbook->BookID && bookData.isdeleted==0){
            printf("DUPLICATE BOOK ID.\n");
            char msg[50]="Duplicate Book ID.";
            send(newsockfd,msg,50,0);
            flock(fd, LOCK_UN); // Unlock
            close(fd);
            return;
        }
    }

    if (found==1){
            // Seek back to the beginning of the record
            lseek(fd, -(off_t)sizeof(struct data), SEEK_CUR);
            if (write(fd, newbook, sizeof(struct data)) == -1) {
                perror("Error writing to database");
                flock(fd, LOCK_UN); // Unlock
                close(fd);
                return;
            }
            char msg[50]="Book added successfully";
            send(newsockfd,msg,50,0);
    }
    else{
        lseek(fd,0,SEEK_END);

        write(fd,&(newbook->BookID),sizeof(int));

        if (write(fd, newbook, REC_SIZE) == -1) {
            perror("Error writing to database");
            flock(fd, LOCK_UN); // Unlock
            close(fd);
            return;
        }
        number_of_records++;
        lseek(fd,0,SEEK_SET);

        write(fd,&number_of_records,sizeof(int));
        
        char response[50]="Book added successfully";
        send(newsockfd, response, strlen(response), 0);
    }

    printf("Book with ID %d added succesfully\n", newbook->BookID);

    flock(fd, LOCK_UN); // Unlock
    close(fd);

}

void deleteBook(int newsockfd){
    printf("\nADMIN OPTION 3 SELECTED : REMOVE A BOOK FROM DATABASE\n");
    int bookID;
    read(newsockfd, &bookID, sizeof(int));

    int fd = open(DATABASE_FILE, O_RDWR);
    if (fd < 0) {
        perror("Error opening database");
        return;
    }

    flock(fd, LOCK_EX); // Exclusive lock for writing

    // Move file pointer to beginning to start reading records
    lseek(fd, 0, SEEK_SET);

    int number_of_records;
    read(fd, &number_of_records, sizeof(int));

    // Search for the book with the given BookID
    struct data bookData;
    int bid;
    int found = 0;
    for (int i = 0; i < number_of_records; i++) {
        read(fd, &bid, sizeof(int));
        if (read(fd, &bookData, sizeof(struct data)) == -1) {
            perror("Error reading database");
            flock(fd, LOCK_UN); // Unlock
            close(fd);
            return;
        }
        if (bookData.BookID == bookID && bookData.isdeleted==0) {
            found = 1;
            printf("FOUND BOOK\n");
            break;
        }
    }
    
    if (!found) {
        printf("Book with ID %d not found\n", bookID);
        // Send message to client indicating book not found
        char msg[] = "Book not found";
        send(newsockfd, msg, sizeof(msg), 0);
    }
    else {
            // Update quantity and write back to the database
            bookData.isdeleted=1;
            // Seek back to the beginning of the record
            lseek(fd, -(off_t)sizeof(struct data), SEEK_CUR);
            if (write(fd, &bookData, sizeof(struct data)) == -1) {
                perror("Error writing to database");
                flock(fd, LOCK_UN); // Unlock
                close(fd);
                return;
            }
            printf("Book with ID %d deleted successfully\n", bookID);
            // Send success message to client
            char msg[] = "Book deleted successfully";
            send(newsockfd, msg, sizeof(msg), 0);
        }

        flock(fd, LOCK_UN); // Unlock
        close(fd);
}

void modifyBook(int newsockfd) {
    printf("\nADMIN OPTION 4 SELECTED : MODIFY A BOOK RECORD\n");
    int fd = open(DATABASE_FILE, O_RDWR);
    if (fd < 0) {
        perror("Error opening database");
        char msg[] = "Error opening database";
        send(newsockfd, msg, sizeof(msg), 0);
        return;
    }

    struct data *newbook = malloc(REC_SIZE);
    if (!newbook) {
        perror("Error allocating memory");
        char msg[] = "Error allocating memory";
        send(newsockfd, msg, sizeof(msg), 0);
        close(fd);
        return;
    }

    if (recv(newsockfd, newbook, sizeof(struct data), 0) <= 0) {
        perror("Error receiving data");
        char msg[] = "Error receiving data";
        send(newsockfd, msg, sizeof(msg), 0);
        free(newbook);
        close(fd);
        return;
    }

    if (flock(fd, LOCK_EX) < 0) {
        perror("Error locking file");
        char msg[] = "Error locking file";
        send(newsockfd, msg, sizeof(msg), 0);
        free(newbook);
        close(fd);
        return;
    }

    if (lseek(fd, 0, SEEK_SET) < 0) {
        perror("Error seeking file");
        char msg[] = "Error seeking file";
        send(newsockfd, msg, sizeof(msg), 0);
        flock(fd, LOCK_UN);
        free(newbook);
        close(fd);
        return;
    }

    int number_of_records;
    if (read(fd, &number_of_records, sizeof(int)) != sizeof(int)) {
        perror("Error reading number of records");
        char msg[] = "Error reading number of records";
        send(newsockfd, msg, sizeof(msg), 0);
        flock(fd, LOCK_UN);
        free(newbook);
        close(fd);
        return;
    }

    struct data bookData;
    int found = 0;
    for (int i = 0; i < number_of_records; i++) {
        int bid;
        if (read(fd, &bid, sizeof(int)) != sizeof(int)) {
            perror("Error reading book ID");
            char msg[] = "Error reading book ID";
            send(newsockfd, msg, sizeof(msg), 0);
            flock(fd, LOCK_UN);
            free(newbook);
            close(fd);
            return;
        }

        if (read(fd, &bookData, sizeof(struct data)) != sizeof(struct data)) {
            perror("Error reading book data");
            char msg[] = "Error reading book data";
            send(newsockfd, msg, sizeof(msg), 0);
            flock(fd, LOCK_UN);
            free(newbook);
            close(fd);
            return;
        }

        if (bid == newbook->BookID) {
            if (bookData.isdeleted) {
                printf("Book with ID %d not found\n", bid);
                char msg[] = "Cannot modify deleted records.";
                send(newsockfd, msg, sizeof(msg), 0);
                flock(fd, LOCK_UN);
                free(newbook);
                close(fd);
                return;
            } else {
                found = 1;
                break;
            }
        }
    }

    if (!found) {
        char response[50];
        sprintf(response, "Book with ID %d not found", newbook->BookID);
        send(newsockfd, response, sizeof(response), 0);
    } else {
        if (lseek(fd, -(off_t)sizeof(struct data), SEEK_CUR) < 0) {
            perror("Error seeking file");
            char msg[] = "Error seeking file";
            send(newsockfd, msg, sizeof(msg), 0);
            flock(fd, LOCK_UN);
            free(newbook);
            close(fd);
            return;
        }

        if (write(fd, newbook, sizeof(struct data)) != sizeof(struct data)) {
            perror("Error writing to database");
            char msg[] = "Error writing to database";
            send(newsockfd, msg, sizeof(msg), 0);
            flock(fd, LOCK_UN);
            free(newbook);
            close(fd);
            return;
        }

        char response[50];
        sprintf(response, "Book with ID %d modified successfully", newbook->BookID);
        send(newsockfd, response, sizeof(response), 0);
    }

    free(newbook);
    flock(fd, LOCK_UN);
    close(fd);
}

