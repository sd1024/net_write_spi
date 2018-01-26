#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#define SERVER_IP "10.0.3.239"
#define BUFSIZE 4096

#define PORT_SIZE_FILE 3420
#define PORT_DATA_FILE 3430
char sizeFile[10]={ }; // = "AABBCCDDEEFF\n0123456789\nTest network write EEPROM\nVersion 0.1\n";
char buf[BUFSIZE];
int dataFile;
 #include <string.h>


 /* reverse:  переворачиваем строку s на месте */
 void reverse(char s[])
 {
     int i, j;
     char c;
     for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
         c = s[i];
         s[i] = s[j];
         s[j] = c;
     }
 }


void itoa(unsigned long int n, char s[])
 {
     int i;
     i = 0;
     do {       /* генерируем цифры в обратном порядке */
         s[i++] = n % 10 + '0';   /* берем следующую цифру */
     } while ((n /= 10) > 0);     /* удаляем */
     s[i] = '\0';
     reverse(s);
 }





int main(int argc, char* argv[])
{
    int sock, ret, socksize;
	unsigned long int sizeFile;
    struct sockaddr_in addr;
	struct sockaddr_in addrsize;
	struct stat statistics;

if (argc < 2){
		printf ("need file");
		return 1;
	}
else {
	


	socksize = socket(AF_INET, SOCK_STREAM, 0);
    if(socksize < 0)
    {
        perror("socket");
        exit(1);
    }

	addrsize.sin_family = AF_INET;
    addrsize.sin_port = htons(PORT_SIZE_FILE); // или любой другой порт...
    //addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	addrsize.sin_addr.s_addr = inet_addr(SERVER_IP);
	//inet_aton(SERVER_IP, addr.sin_addr.s_addr);
	//printf("addr %d\n",INADDR_LOOPBACK);
	printf("addrsize %x\n",addrsize.sin_addr.s_addr);

	if(connect(socksize, (struct sockaddr *)&addrsize, sizeof(addrsize)) < 0)
    {
        perror("connect size");
        exit(2);
    }
	
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
    {
        perror("socket size");
        exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT_DATA_FILE); // или любой другой порт...
    //addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	addr.sin_addr.s_addr = inet_addr(SERVER_IP);
	//inet_aton(SERVER_IP, addr.sin_addr.s_addr);
	//printf("addr %d\n",INADDR_LOOPBACK);
	printf("addr %x\n",addr.sin_addr.s_addr);
    if(connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("connect file");
        exit(2);
    }

	if ((dataFile = open(argv[1], O_RDONLY))==-1) {
		printf("Can't open file %s\n",argv[1]);
		return 1;
	} else {
		if (fstat(dataFile, &statistics) != -1) {
				itoa(statistics.st_size,sizeFile);
				printf("size file %s\n",sizeFile);
                send(socksize, sizeFile, sizeof(sizeFile), 0);
            } else {
				perror("error size file");	
				exit(2);
			}
		
		while ((ret = read(dataFile, buf, BUFSIZE)) > 0) {
			//printf(buf);
			printf("Send byte: %d\n",ret);
			send(sock, buf, ret, 0);
		}
	if (ret < 0)
		printf("read error");

	}
    
    //recv(sock, buf, sizeof(message), 0);
    
    
    close(sock);

    return 0;
}
}
