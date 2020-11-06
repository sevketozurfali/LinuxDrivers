#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

int8_t write_buf[1024];
int8_t read_buf[1024];


int main(){
    int fd;
    char option;

    printf("Welcome to the char device driver test app....\n");

    fd = open("/dev/techo_device", O_RDWR);
    if(fd < 0){
        printf("Device file cannot open.\n");
        return 0;
    }

    while(1){
        printf("File opened sucessfully.\n");

        printf("*********Please select your option : ******************\n");

        printf("                   1 : Write                           \n");
        printf("                   2 : Read                            \n");
        printf("                   3 : Exit                            \n");
        scanf(" %c", &option);

        printf("Your option : %c\n", option);


        switch (option)
        {
        case '1':
            printf("Enter the string want to write to driver : \n");
            scanf(" %[^\t\n]s", write_buf);
            printf("Data written.....\n");
            write(fd, write_buf, strlen(write_buf)+1);
            printf("Done.\n");
            break;
        
        case '2': 
            printf("Data reading......\n");
            read(fd, read_buf, 1024);
            printf("Done. \n");
            printf("Data : %s\n\n", read_buf);
            break;

        case '3':
            close(fd);
            exit(1);

        default:
            printf("Please enter the valid option : %c \n ", option);
            break;
        }


    }

    close(fd);

}