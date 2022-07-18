#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define WR_DATA _IOW('a','a',int32_t*)
#define RD_DATA _IOR('a','a',int32_t*)

int8_t write_buf[1024];
int8_t read_buf[1024];

int fd, selection;
char option;
int32_t val, num;

void file_read_write();
void ioctl_read_write();

int main(){
    

    printf("Welcome to the char device driver test app....\n");

    while(1){

        printf("How would you like to use this driver please select : \n");
        printf("----------1 To file read write----------------\n");
        printf("----------2 To IOCTL read write---------------\n");
        scanf("%d", &selection);

        switch (selection)
        {
        case 1:
            file_read_write();
            break;

        case 2:
            ioctl_read_write();

        case 3:
            exit(1);

        default:
            break;
        }

    }
    

}


void file_read_write(){

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



void ioctl_read_write(){

    fd = open("/dev/techo_device", O_RDWR);
    if(fd < 0){
        printf("Device file cannot open.\n");
        return 0;
    }

    printf("Enter data to send....\n");

    scanf("%d", &num);

    printf("Writing data to driver ...");

    ioctl(fd, WR_DATA, (int32_t*)&num);
    printf("Reading data from driver... \n");

    ioctl(fd, RD_DATA, (int32_t*)&val);
    printf("Data is : %d\n",val);
    printf("Closing the driver....");
    close(fd);
}