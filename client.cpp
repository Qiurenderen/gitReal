#include <sys/socket.h>
#include <stdio.h>      //perror
#include <netinet/in.h> //sockaddr_in, htons
#include <arpa/inet.h>  //inet_pton
#include <string.h>     //bzero
#include <unistd.h>     //close
#include <iostream>
#include "network.hpp"

#define MAXLEN 100

void p_sockaddr(const sockaddr_in *addr){
	for (size_t i=0; i<sizeof(sockaddr_in); ++i){
		const char *p = (const char*)(addr);
		std::cout << (int)(*p) << std::endl;
		p += i;
	}
}

int main(int argc, char *argv[]){
	if (argc < 2){
		std::cerr << "usage ./client <server IP> " << std::endl;
		exit(0);
	}

	int sockfd;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1){
		perror("The following error occurred, socket: ");
		exit(1); //need to understand exit code vs errno
	}

	struct sockaddr_in addr;
	//p_sockaddr(&addr);
	bzero(&addr, sizeof(addr));
	//p_sockaddr(&addr); //evidenced that bzero took effect
	addr.sin_family = PF_INET;
	addr.sin_port = htons(9800);
	inet_pton(AF_INET, argv[1], &(addr.sin_addr));

/*
    // binding a client port
    struct sockaddr_in cli_addr;
    cli_addr.sin_family = PF_INET;
    cli_addr.sin_port = htons(61234);

    if (bind(sockfd, (struct sockaddr*)(&cli_addr), sizeof(cli_addr)) == -1){
        perror("The following error occurred, bind: ");
        exit(1);
    }
*/
	if (connect(sockfd, (struct sockaddr*) &addr, sizeof(addr)) == -1){
		perror("The following error occurred, connect: ");
		exit(1);
	}

	//str_cli(stdin, sockfd);
    char    sendline[MAXLEN], recvline[MAXLEN];
	while (fgets(sendline, MAXLEN, stdin) != NULL) {
        writen(sockfd, sendline, strlen(sendline));
        int n = read(sockfd, recvline, MAXLEN);
		if(n==1) {
			//shutdown(sockfd, SHUT_WR);
			close(sockfd);
			break;
		}
        else if(n>0) {
            recvline[std::min(n, MAXLEN-1)]=0;
            fputs(recvline, stdout);
            std::cerr << std::endl << "=========" << std::endl;
            std::cerr << "n=" << n << std::endl;
            std::cerr << (int)recvline[0] << std::endl;
        }
        else{
            perror("str_cli error: ");
            return 1;
        }
    }

	std::cerr << "please input a char for close call, check client TCP status before and after" << std::endl;
	char c;
	std::cin >> c;
	shutdown(sockfd, SHUT_WR);
	std::cerr << "please input a char, now close called" << std::endl;
	std::cin >> c;
}

