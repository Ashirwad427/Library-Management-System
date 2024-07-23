#include <stdio.h>
#include<string.h>
#include<errno.h>
#include<stdlib.h>
#include "user.h"

int main(){
    char filename[]="admins.dat";
    FILE* data_fp=fopen(filename,"wb+");
    int rec_count=4;

    // Creating 2 struct objects
    struct credential user1,user2,user3,user4;
    int rec_size=sizeof(user1);

    strcpy(user1.username,"ashirwad");
    strcpy(user1.password,"abcd");
    
    strcpy(user2.username,"carlo");
    strcpy(user2.password,"ancelotti");

    strcpy(user3.username,"jose");
    strcpy(user3.password,"mourinho");

    strcpy(user4.username,"sergio");
    strcpy(user4.password,"ramos");

    fseek(data_fp,0,SEEK_SET);

    fwrite(&rec_count,sizeof(int),1,data_fp);

    fwrite(&user1, rec_size, 1, data_fp);

    fwrite(&user2, rec_size, 1, data_fp);

    fwrite(&user3, rec_size, 1, data_fp);

    fwrite(&user4, rec_size, 1, data_fp);

    fclose(data_fp);
}