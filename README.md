# Library-Management-System

Project Synopsis
The provided codebase implements a basic Library Management System (LMS) using a client-server architecture. The system allows users to interact with a library database through a client application, which communicates with a server to perform various library-related operations. The system supports two user roles: a regular user and an administrator.

System Architecture

The system is composed of two main components:

Client: A command-line interface application that allows users to interact with the system. It handles user authentication, displays menus, and sends user requests to the server.
Server: A multi-threaded server that listens for incoming client connections, handles authentication, processes client requests, and manages the library database.
Functionality

Client-side functionality:

User authentication (login) for both users and administrators.
User interface for displaying menus and options.
Input handling for user commands.
Network communication with the server.

Server-side functionality:

Network socket creation and management.
Client connection handling and thread management.
User authentication against user and administrator databases.
Database management (reading, writing, updating book records).
Handling client requests for various library operations (searching, borrowing, returning, adding, modifying, deleting books).

Data Storage

The system uses file-based storage for the library database and user/administrator credentials. The database file stores book information, including book ID, author, title, quantity, and genre. User and administrator credentials are stored in separate files.

Communication

Client and server communicate using TCP sockets. The client sends user input and receives responses from the server.

Threading

The server utilizes multi-threading to handle multiple client connections concurrently. Each client connection is handled by a separate thread.

