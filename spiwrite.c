/*
 * main.c
 *
 *  Created on: Nov 27, 2017
 *      Author: sd
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <fcntl.h>
//#include <signal.h>
//#include <unistd.h>

#define BUFSIZE 4096

/*
#define FSBL_FILE_NAME    	"/dev/mtd0"
#define UIMAGE_FILE_NAME   	"/dev/mtd1"
#define DTB_FILE_NAME   	"/dev/mtd2"
#define ROOTFS_FILE_NAME   	"/dev/mtd3"
#define BIT_FILE_NAME   	"/dev/mtd4"
#define BUF_FILE_NAME0    	"temp_data0"
#define BUF_FILE_NAME1    	"temp_data1"
#define BUF_FILE_NAME2    	"temp_data2"
#define BUF_FILE_NAME3    	"temp_data3"
#define BUF_FILE_NAME4    	"temp_data4"
*/
#define SPI_FILE_NAME    	"/dev/mtd0"
#define BUF_FILE_NAME    	"temp_data0"
#define PORT_SIZE_FILE    	3420
#define PORT_DATA_FILE    	3430
#define COMMAND_WRITE_FLASH "flashcp temp_data0 /dev/mtd0"
#define PERMS	0777

int spiFlash_w;
int spiFlash_r;
int dataFile;
int readFile;
//long int fileSize;

char buf[BUFSIZE];
char fileSize[10];

int write_spi(){
    int sockfile, socksize, listener;
    int listenerSize;
    long int ost=0;
    int status;
    struct sockaddr_in addr;
    struct sockaddr_in addrsize;
    //char buf[1024];
    int bytes_read;
    int bytes_size;
    listenerSize = socket(AF_INET, SOCK_STREAM, 0);
	if(listenerSize < 0)
	    {
	        perror("socket");
	        exit(1);
	    }
	    addrsize.sin_family = AF_INET;
	    addrsize.sin_port = htons(PORT_SIZE_FILE);
	    addrsize.sin_addr.s_addr = htonl(INADDR_ANY);
	    if(bind(listenerSize, (struct sockaddr *)&addrsize, sizeof(addrsize)) < 0)
	    {
	        perror("bind");
	        exit(2);
	    }


	    listener = socket(AF_INET, SOCK_STREAM, 0);
	    if(listener < 0)
	    {
	        perror("socket");
	        exit(1);
	    }

	    addr.sin_family = AF_INET;
	    addr.sin_port = htons(PORT_DATA_FILE);
	    addr.sin_addr.s_addr = htonl(INADDR_ANY);
	    if(bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	    {
	        perror("bind");
	        exit(2);
	    }
	    listen(listenerSize, 1);
	    listen(listener, 1);

	    while(1)
	    {
	    	printf("listening...\n");
	    	socksize = accept(listenerSize, NULL, NULL);

	    	if(socksize < 0)
	        {
	            perror("accept");
	            exit(3);
	        }
	        if ((bytes_size=recv(socksize, fileSize, 10, 0)) > 0) {
	            printf("file size: %s\n",fileSize);
	        } else
				printf("Size error");

			ost = atol(fileSize);
			//printf("Ostatok: %d\n",ost);
	        sockfile = accept(listener, NULL, NULL);

	        if(sockfile < 0)
	        {
	            perror("accept");
	            exit(3);
	        }

	        if ((spiFlash_w = open(BUF_FILE_NAME, O_WRONLY | O_CREAT | O_TRUNC, PERMS))==-1) {
	            printf("Can't open file %s\n", BUF_FILE_NAME);
	            return 1;
	        } else {
	            printf("start read file\n");

	            while ((ost>0) && ((bytes_read = recv(sockfile, buf, BUFSIZE, 0)) > 0) ) {
	            	//printf("Write byte: %d\n",bytes_read);
	            	ost=ost-bytes_read;
	            	//printf("Ostatok: %d\n",ost);

	            	if (write(spiFlash_w, buf, bytes_read) != bytes_read)
	            		printf("Write error");
	            }
	            printf("end read file\n");
	            //if (bytes_read < 0)
	            	//printf("Write error");

	            printf("close socket\n");
	        //fsync(spiFlash_w);
	        close(socksize);
	        close(sockfile);
	        close(spiFlash_w);
	        //printf(strcat("flashcp ", strcat(strcat(BUF_FILE_NAME," "),SPI_FILE_NAME)));
	        printf("write_device...\n");
	        status = system(COMMAND_WRITE_FLASH);
	        if (status = system(COMMAND_WRITE_FLASH) < 0)
	        	printf("ошиб­ка вы­зо­ва функ­ции system()\n");

	        printf("write complete: %d\n", status);

	        }

	    }

	    return 0;
	}

int main()
{
    	write_spi();

    return 0;
}



