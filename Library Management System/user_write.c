#include <stdio.h>
#include<string.h>
#include<errno.h>
#include<stdlib.h>
#include "user.h"

int main(){
    char filename[]="users.dat";
    FILE* data_fp=fopen(filename,"wb+");
    int rec_count=5;

    // Creating 5 users
    struct credential user1,user2,user3,user4,user5;
    int rec_size=sizeof(user1);

    strcpy(user1.username,"ashirwad");
    strcpy(user1.password,"abcd");
    
    strcpy(user2.username,"sergio");
    strcpy(user2.password,"ramos");

    strcpy(user3.username,"luka");
    strcpy(user3.password,"modric");

    strcpy(user4.username,"cristiano");
    strcpy(user4.password,"ronaldo");

    strcpy(user5.username,"gareth");
    strcpy(user5.password,"bale");



    fseek(data_fp,0,SEEK_SET);

    fwrite(&rec_count,sizeof(int),1,data_fp);

    fwrite(&user1, rec_size, 1, data_fp);

    fwrite(&user2, rec_size, 1, data_fp);

    fwrite(&user3, rec_size, 1, data_fp);

    fwrite(&user4, rec_size, 1, data_fp);

    fwrite(&user5, rec_size, 1, data_fp);

    fclose(data_fp);

}