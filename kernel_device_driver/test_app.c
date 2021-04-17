#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/ioctl.h>

int8_t write_buf[1024];
int8_t read_buf[1024];

#define WR_DATA _IOW('a','a',int32_t*)
#define RD_DATA _IOR('a','b',int32_t*)

int simple_driver(){
        int fd;
    char option;

    printf("Welcome to the demo of character device driver.\n");

    fd = open("/dev/my_device", O_RDWR);
    if(fd < 0){
        printf("Can't open device file.\n");
        return 0;
    }

    while (1)
    {
        printf("**************Please Enter your option*********************\n");
        printf("1. Write \n");
        printf("2. Read \n");
        printf("3. Exit \n");
        scanf("%c",&option);

        printf("Your option is : %c\n", option);

        switch (option)
        {
        case '1':
            printf("Enter the string to write into the driver ");
            scanf("%[^\t\n]s", write_buf);
            printf("Data written...\n");
            write(fd, write_buf, strlen(write_buf)+1);
            printf("DONE...\n");
            break;

        case '2':
            printf("Data is Reading ... \n");
            read(fd, read_buf, 1024);
            printf("Done ... \n\n");
            printf("Data = %s\n\n", read_buf);
            break;

        case '3':
            close(fd);
            exit(1);
            break;

        
        default:
            printf("Enter a valid option : %c\n", option);
            break;
        }
    }

    close(fd);
    
}

int driver_with_ioctl(){
    int fd;
    int32_t val, num;
    printf("IoCTL based Character device driver operation from user space... \n");

    fd = open("/dev/my_device", O_RDWR);
    if(fd < 0){
        printf("IOCTL Device cant open...\n");
        return 0;

    }

    printf("Enter the data to send....\n");
    scanf("%d", &num);

    printf("Writing value data with ioctl...\n");
    ioctl(fd, WR_DATA, (int32_t*)&num);

    printf("Reading data from driver\n");

    ioctl(fd, RD_DATA, (int32_t*)&val);

    printf("Value : %d\n", val);
    printf("Closing driver...\n");

    close(fd);




}

int main(){
    //simple_driver();

    driver_with_ioctl();
}
