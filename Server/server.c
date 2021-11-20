#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "../src/hex_to_ascii.h"
#include "../src/mesg_to_decipher_convert.h"
#define MAX 2000
#define PORT 8080
#define SA struct sockaddr

void convertHexToAscii(char *str){
    int length = strlen(str);
    int i;
    char buf = 0;
    for(i = 0; i < length; i++){
        if(i % 2 != 0){
            printf("%c", hex_to_ascii(buf, str[i]));
        }else{
            buf = str[i];
        }
    }
}
// Function designed for chat between client and server.
void func(int sockfd)
{	
	uint8_t key[] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 
					0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };
	char buff[MAX];
	char* output = (char*)malloc(100* sizeof(char));
	int n;
	int valread;
	// infinite loop for chat
	for (;;) {
		bzero(buff, sizeof(buff));
		bzero(output, strlen(output));
		// read the message from client and copy it in buffer
		if(valread = read(sockfd, buff, sizeof(buff))){
			printf("From client: %s\n", buff);
		} else{
			printf("Read fail!\n");
		}
		// printf("\nMessenger size: %d\n",valread);
		output = decipherConvertMessenger(key,buff);
		// printf("%s\n",output);
		printf("Messenger from client: ");
		convertHexToAscii(output);
		printf("\n");
		// print buffer which contains the client contents
		// if msg contains "Exit" then server exit and chat ended.
		if (strncmp("exit", output, 4) == 0) {
			printf("Server Exit...\n");
			break;
		}
	}
}

// Driver function
int main()
{
	int sockfd, connfd, len;
	struct sockaddr_in servaddr, cli;

	// socket create and verification
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("socket creation failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully created..\n");
	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);

	// Binding newly created socket to given IP and verification
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
		printf("socket bind failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully binded..\n");

	// Now server is ready to listen and verification
	if ((listen(sockfd, 5)) != 0) {
		printf("Listen failed...\n");
		exit(0);
	}
	else
		printf("Server listening..\n");
	len = sizeof(cli);

	// Accept the data packet from client and verification
	connfd = accept(sockfd, (SA*)&cli, &len);
	if (connfd < 0) {
		printf("server accept failed...\n");
		exit(0);
	}
	else
		printf("server accept the client...\n");

	// Function for chatting between client and server
	func(connfd);

	// After chatting close the socket
	close(sockfd);
}
