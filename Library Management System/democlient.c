
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "data.h"
#include "user.h"

#define BUFFER_SIZE 255

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <ip_address> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *ip_address = argv[1];
    int portno = atoi(argv[2]);
    int sockfd;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];
    char input[BUFFER_SIZE];

    printf("\nCREATING SOCKET...\n");
    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error opening socket");
        exit(EXIT_FAILURE);
    }

    // Initialize socket structure
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);

    // Convert IP address to binary form
    if (inet_pton(AF_INET, ip_address, &serv_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    printf("\nESTABLISHING CONNECTION WITH SERVER...\n");
    // Connect to server
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    printf("\nCONNECTION ESTABLISHED SOCKET-ID : %d\n", sockfd);

    // Authentication loop (simplified)
    char user_type;
    while (1) {
        printf("\nChoose Login type: \n");
        printf("1. USER LOGIN\n");
        printf("2. ADMINISTRATOR LOGIN\n");
        printf("3. EXIT");
        printf("\nENTER YOUR CHOICE (1-3): ");
        scanf(" %c", &user_type);
        send(sockfd, &user_type, sizeof(user_type), 0);

        if(user_type=='3'){
            printf("\nBye! Have a Nice day. \n");
            close(sockfd);
            return 0;
        }
        printf("Enter username: ");
        scanf("%s", input);
        send(sockfd, input, strlen(input)+1, 0);

        printf("Enter password: ");
        scanf("%s", input);
        send(sockfd, input, strlen(input)+1, 0);

        read(sockfd, buffer, BUFFER_SIZE);
        printf("%s\n", buffer);
        
        if (strcmp(buffer, "User Login successful\n") == 0 || strcmp(buffer, " Administrator Login successful\n") == 0) {
            break;
        }
        else{
            close(sockfd);
            return(0);
        }
    }
    if(user_type=='1'){
    printf("\n Hello User! Welcome to the Online Library Management System!\n ");
    while(1){
        char option;
        char intro_message[]="\nWhat would you like to do? \n1. Get all books and their details \n2. Issue a book\n3. Return a book\n4. Exit\nEnter your choice (1-4): ";
        printf("%s",intro_message);
        scanf(" %c",&option);

        send(sockfd,&option,sizeof(option),0);

        if(option=='4'){
            printf("\nBye. Have a nice day!\n");
            break;
        }
        else if(option=='1'){
            printf("\nFetching records...\n \n");

            // Receive the number of records from the server
            int number_of_records;
            if (recv(sockfd, &number_of_records, sizeof(int), 0) == -1) {
                perror("Error receiving number of records from server");
                continue;
            }

            // Check if there are no records
            if (number_of_records == 0) {
                printf("No records found.\n");
                continue;
            }

            // Allocate or reallocate memory to receive the records
            struct data *received_records = NULL;
            received_records = realloc(received_records, number_of_records * REC_SIZE);
            if (received_records == NULL) {
                perror("Memory allocation failed");
                continue;
            }

            // Receive the data from the server
            int bytes_received = recv(sockfd, received_records, number_of_records * REC_SIZE, 0);
            if (bytes_received == -1) {
                perror("Error receiving data from server");
                free(received_records);
                continue;
            } else if (bytes_received == 0) {
                printf("Connection closed by server\n");
                free(received_records);
                continue;
            }

            // Process the received records
            int num_records_received = bytes_received / REC_SIZE;
            for(int i = 0; i < num_records_received; i++) {
                if(received_records[i].isdeleted==0){
                    printf("Book ID - %d Author Name - %s Title - %s Quantity - %d Genre - %s\n", received_records[i].BookID, received_records[i].AuthorName, received_records[i].BookTitle, received_records[i].Qty, received_records[i].Genre);
                }
            }

            printf("\n All records fetched \n");

            // Free dynamically allocated memory
            free(received_records);
        }

        else if(option=='2'){
                int bookID;
                printf("Enter the Book ID to borrow: ");
                scanf("%d", &bookID);
                send(sockfd, &bookID, sizeof(int), 0);
                
                printf("\n");
                // Receive response from server
                char response[BUFFER_SIZE];
                recv(sockfd, response, BUFFER_SIZE, 0);
                printf("%s\n", response);
        }
        else if (option == '3') {
            int bookID;
            printf("Enter the Book ID to return: ");
            scanf("%d", &bookID);
            send(sockfd, &bookID, sizeof(int), 0);

            printf("\n");
            // Receive response from server
            char response[BUFFER_SIZE];
            recv(sockfd, response, BUFFER_SIZE, 0);
            printf("%s\n", response);
        }
        else{
            printf("\n Invalid option. Choose only from 1-4 \n");
        }
    }
    }
    else if(user_type=='2'){
    printf("\n Hello Administrator! Welcome to the Online Library Management System!\n ");
    while(1){
        char option;
        char intro_message[]="\n What would you like to do?\n1. Get information about a book\n2. Add a new book\n3. Remove an existing book\n4. Modify book details\n5. Get user details\n6. Get all book details\n7. Exit \n \nEnter your choice (1-7): ";
        printf("%s",intro_message);
        scanf(" %c",&option);

        send(sockfd,&option,sizeof(option),0);

        if(option=='7'){
            printf("Bye. Have a nice day!\n");
            break;
        }

        else if (option == '1') {
            int bookID;
            printf("\nEnter the Book ID: ");
            scanf("%d", &bookID);
            send(sockfd, &bookID, sizeof(int), 0);

            struct data *received_record = malloc(sizeof(struct data));
            if (received_record == NULL) {
                perror("Memory allocation failed");
                continue;
            }

            int bytes_received = recv(sockfd, received_record, sizeof(struct data), 0);
            if (bytes_received == -1) {
                perror("Error receiving data from server");
                free(received_record);
                continue;
            } else if (bytes_received == 0) {
                printf("Connection closed by server\n");
                free(received_record);
                continue;
            } else if (strcmp(received_record->AuthorName, "Book not found") == 0) {
                printf("\nBook not found\n");
                free(received_record);
                continue;
            }

            printf("\nFetching your record...\n");

            printf("\nBook ID - %d Author Name - %s Title - %s Quantity - %d Genre - %s\n", 
                    received_record->BookID, received_record->AuthorName, 
                    received_record->BookTitle, received_record->Qty, 
                    received_record->Genre);

            printf("\nRecord fetch complete.\n");

            free(received_record);
        }



        else if (option == '2') {
            // Allocate memory for the new book struct
            struct data *newbook = malloc(sizeof(struct data));
            if (newbook == NULL) {
                perror("Memory allocation failed");
                exit(EXIT_FAILURE);
            }

            // Input book details
            printf("\nEnter the new book details (Give a new,unique Book ID and details)\n");
            printf("Book ID: ");
            scanf("%d", &(newbook->BookID));

            while (getchar() != '\n');

            printf("Author Name: ");
            fgets(newbook->AuthorName, A_NAME, stdin);
            newbook->AuthorName[strcspn(newbook->AuthorName, "\n")] = '\0'; // Remove trailing newline if any

            printf("Book Title: ");
            fgets(newbook->BookTitle, TITLE, stdin);
            newbook->BookTitle[strcspn(newbook->BookTitle, "\n")] = '\0'; // Remove trailing newline if any

            printf("Quantity (Number of books): ");
            scanf("%d", &(newbook->Qty));

            while (getchar() != '\n');

            printf("Genre: ");
            fgets(newbook->Genre, GENRE, stdin);
            newbook->Genre[strcspn(newbook->Genre, "\n")] = '\0'; // Remove trailing newline if any

            newbook->isdeleted = 0;

            // Send the new book record to the server
            int bytes_sent = send(sockfd, newbook, sizeof(struct data), 0);
            if (bytes_sent == -1) {
                perror("Error sending data to server");
            } else if (bytes_sent == sizeof(struct data)) {
                printf("\nSending record to Database\n");
                printf("\n Awaiting response...\n");
            }

            char response[50];
            read(sockfd,response,50);
            if(strcmp(response,"Duplicate Book ID.")==0){
                printf("\nDuplicate Book ID.\n");
            }
            else if(strcmp(response,"Book added successfully")==0){
                printf("\n");
                printf("Book added succesfully\n");
            }

            // Free the allocated memory
            free(newbook);
        }
        else if(option=='3'){
            int bookID;
            printf("\nEnter the Book ID: ");
            scanf("%d", &bookID);
            send(sockfd, &bookID, sizeof(int), 0);

            char msg[100]; // Assuming the maximum message length is 100 characters
            recv(sockfd, msg, sizeof(msg), 0); // Receive message from server

            if (strcmp(msg, "Book deleted successfully") == 0) {
                printf("\nSuccessfully deleted book record from database\n");
            }
            else if(strcmp(msg,"Book not found")==0){
                printf("\nBook not found\n");
            }
        }
        else if (option == '4') {
            struct data newbook;

            // Input book details
            printf("\nEnter the modified details (Cannot modify 'Book ID', only modifies attributes of a specific 'Book ID')\n");
            printf("Book ID: ");
            scanf("%d", &(newbook.BookID));
            while (getchar() != '\n'); // Clear input buffer

            printf("Author Name: ");
            fgets(newbook.AuthorName, 100, stdin);
            newbook.AuthorName[strcspn(newbook.AuthorName, "\n")] = '\0'; // Remove trailing newline if any

            printf("Book Title: ");
            fgets(newbook.BookTitle, 100, stdin);
            newbook.BookTitle[strcspn(newbook.BookTitle, "\n")] = '\0'; // Remove trailing newline if any

            printf("Quantity (Number of books): ");
            scanf("%d", &(newbook.Qty));
            while (getchar() != '\n'); // Clear input buffer

            printf("Genre: ");
            fgets(newbook.Genre, 50, stdin);
            newbook.Genre[strcspn(newbook.Genre, "\n")] = '\0'; // Remove trailing newline if any

            newbook.isdeleted = 0;

            // Send the new book record to the server
            if (send(sockfd, &newbook, sizeof(struct data), 0) < 0) {
                perror("Error sending data to server");
                close(sockfd);
                exit(EXIT_FAILURE);
            }

            // Receive response from server
            char response[50];
            int bytes_received = recv(sockfd, response, sizeof(response) - 1, 0);
            if (bytes_received > 0) {
                response[bytes_received] = '\0'; // Null-terminate the response
                printf("\n%s\n", response);
            } else {
                perror("Error receiving response from server");
            }
        }
        else if(option=='5'){

            FILE *data_fp;
            char filename[] = "users.dat";
            int rec_count;
            struct credential user;
            int rec_size = sizeof(user);

            // Open the file for reading
            data_fp = fopen(filename, "rb");
            if (data_fp == NULL) {
                perror("Error opening file");
            }
            
            fseek(data_fp, 0, SEEK_SET);

            // Read the number of records
            if (fread(&rec_count, sizeof(int), 1, data_fp) != 1) {
                perror("Error reading record count");
                fclose(data_fp);
            }

            // Allocate buffer to hold all records
            struct credential *users = malloc(rec_size * rec_count);
            if (users == NULL) {
                perror("Error allocating memory");
                fclose(data_fp);
            }

            // Read all records into the buffer
            if (fread(users, rec_size, rec_count, data_fp) != rec_count) {
                perror("Error reading user records");
                free(users);
                fclose(data_fp);
            }
            // Print received records with good formatting
            printf("Received %d user records:\n", rec_count);
            for (int i = 0; i < rec_count; i++) {
                printf("User %d - ", i + 1);
                printf("  Username: %s ", users[i].username);
                printf("  Password: %s\n", users[i].password);
            }

            // Free allocated memory
            free(users);
            fclose(data_fp);
        }
        else if(option=='6'){
            printf("\nFetching records...\n \n");

            // Receive the number of records from the server
            int number_of_records;
            if (recv(sockfd, &number_of_records, sizeof(int), 0) == -1) {
                perror("Error receiving number of records from server");
                continue;
            }

            // Check if there are no records
            if (number_of_records == 0) {
                printf("No records found.\n");
                continue;
            }

            // Allocate or reallocate memory to receive the records
            struct data *received_records = NULL;
            received_records = realloc(received_records, number_of_records * REC_SIZE);
            if (received_records == NULL) {
                perror("Memory allocation failed");
                continue;
            }

            // Receive the data from the server
            int bytes_received = recv(sockfd, received_records, number_of_records * REC_SIZE, 0);
            if (bytes_received == -1) {
                perror("Error receiving data from server");
                free(received_records);
                continue;
            } else if (bytes_received == 0) {
                printf("Connection closed by server\n");
                free(received_records);
                continue;
            }

            // Process the received records
            int num_records_received = bytes_received / REC_SIZE;
            for(int i = 0; i < num_records_received; i++) {
                if(received_records[i].isdeleted==0){
                    printf("Book ID - %d Author Name - %s Title - %s Quantity - %d Genre - %s\n", received_records[i].BookID, received_records[i].AuthorName, received_records[i].BookTitle, received_records[i].Qty, received_records[i].Genre);
                }
            }

            printf("\n All records fetched \n");

            // Free dynamically allocated memory
            free(received_records);
        }
        else{
            printf("\n Invalid option. Choose only from 1-7 \n");
        }

    }

    close(sockfd);
    return 0;
    }
}