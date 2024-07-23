#ifndef USER_H
#define USER_H

#define USERNAME_SIZE 30
#define PASSWORD_SIZE 50

struct credential{
    char username[USERNAME_SIZE];
    char password[PASSWORD_SIZE];
};

#endif