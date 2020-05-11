#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>

int8_t write_buf[1024];
int8_t read_buf[1024];

int main(){
	int fd;
	char option;

	printf("welcome to the demo of character device driver....\n");

	fd = open("/dev/my_device",O_RDWR);

	if(fd < 0){

		printf("Cannot open the device file \n");
		return 0;

	}

	while (1)
	{
		printf("*************Please enter your options***********\n");
		printf("            1. writer                            \n");
		printf("            2. read                            \n");
		printf("            3. exit                            \n");
		scanf("%c", &option);
		printf("Your option is %c\n", option);

		switch(option){
			case '1':
			printf("Enter the string to write into the driver.\n");
			scanf("%[^\t\n]s", write_buf);
			printf("Data written.\n");
			write(fd, write_buf, strlen(write_buf) + 1);
			printf("Done\n");
			break;
			case '2':
			printf("Data reading....");
			read(fd,read_buf, 1024);
			printf("Done .... \n\n");
			printf("Data = %s\n\n", read_buf);
			break;
			case '3':
			close(fd);
			exit(1);
			break;
			default:
			printf("Enter  valid option = %c\n", option);
			break;

		}

	}
	close(fd);
}
	
