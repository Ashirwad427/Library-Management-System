#ifndef DATA_H
#define DATA_H

#define REC_SIZE 84
#define TITLE 30
#define A_NAME 20
#define GENRE 20

struct data{
    int BookID;
    char BookTitle[TITLE];
    char AuthorName[A_NAME];
    char Genre[GENRE];
    int Qty;
    int isdeleted;      //1 - deleted and 0 - not deleted
};

#endif