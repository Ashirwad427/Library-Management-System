## Project Overview: Library Management System

### Introduction
This project implements a basic Library Management System (LMS) using a client-server architecture. The client application allows users to interact with a library database through a server.

### Functionality
* **User Roles:**
  * **User:** Can search for books, issue books, and return books.
  * **Administrator:** Can add, modify, and delete books, view user details, and get complete book information.
* **Client-Server Interaction:**
  * The client establishes a network connection with the server.
  * The client authenticates the user and displays a menu based on the user's role.
  * The client sends user input to the server and receives responses.

### Code Structure
* **Client-side:**
  * Handles user interaction, network communication, and data display.
* **Server-side:**
  * Manages network connections, user authentication, database operations, and client requests.

### Key Features
* Client-server architecture for distributed processing
* User authentication with role-based access control
* Basic book management functionalities (search, issue, return, add, modify, delete)
* User management (view user details)
* Network communication using sockets
