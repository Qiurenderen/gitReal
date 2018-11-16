#include <sys/socket.h>
#include <stdio.h>      //perror
#include <netinet/in.h> //sockaddr_in, htons
#include <arpa/inet.h>  //inet_pton
#include <string.h>     //bzero
#include <iostream>
#include <string>
#include "network.hpp"
#include <poll.h>
#include<signal.h>

void sig_handler(int signo)
{
  if (signo == SIGPIPE)
    printf("received SIGPIPE\n");
}


void p_local_sockaddr(int sockfd, const std::string str = ""){
		// the listenfd peer side always has IP 0 and peer 0
	char buff[MAXLEN];
	struct sockaddr_in addr;
	bzero(&addr, sizeof(addr));
	unsigned int len = sizeof(addr);
	getsockname(sockfd, (struct sockaddr*) &addr, &len);

	if (inet_ntop(AF_INET, &(addr.sin_addr), buff, sizeof(buff)) != NULL){
		std::cout << str << " address " << buff << " port " << ntohs(addr.sin_port) << std::endl;
	}
	else{
		perror("The following error occurred, inet_ntop: ");
	}
}

void p_sockaddr_byte(const sockaddr_in *addr){
	for (size_t i=0; i<sizeof(sockaddr_in); ++i){
		const char *p = (const char*)(addr);
		std::cout << (int)(*p) << std::endl;
		p += i;
	}
}

#define FLAG_CHECK(revents, flag) { if(revents & flag) {std::cout << #flag << std::endl;}}

void check_flag(short revents){
	FLAG_CHECK(revents, POLLIN)
	FLAG_CHECK(revents, POLLRDNORM)
	FLAG_CHECK(revents, POLLRDBAND)
	FLAG_CHECK(revents, POLLPRI)
	FLAG_CHECK(revents, POLLRDHUP)
	FLAG_CHECK(revents, POLLOUT)
	FLAG_CHECK(revents, POLLWRNORM)
	FLAG_CHECK(revents, POLLWRBAND)
	FLAG_CHECK(revents, POLLERR)
	FLAG_CHECK(revents, POLLHUP)
	FLAG_CHECK(revents, POLLNVAL)
}

int main(int argc, char *argv[]){

	if (signal(SIGPIPE, sig_handler) == SIG_ERR) {
		printf("\ncan't catch SIGINT\n");
	}

	int listenfd;
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd == -1){
		perror("The following error occurred, socket: ");
		exit(1); //need to understand exit code vs errno
	}

	struct sockaddr_in addr;
	//p_sockaddr_byte(&addr);
	bzero(&addr, sizeof(addr));
	//p_sockaddr_byte(&addr); //evidenced that bzero took effect
	addr.sin_family = PF_INET;
	addr.sin_port = htons(9800);
	addr.sin_addr.s_addr = htonl(INADDR_ANY); //INADDR_ANY is zero, htonl not needed

	p_local_sockaddr(listenfd, "initialize listenfd ");

	if (bind(listenfd, (struct sockaddr*)(&addr), sizeof(addr)) == -1){
		perror("The following error occurred, bind: ");
		exit(1);
	}

	p_local_sockaddr(listenfd, "after bind listenfd has address,port INADDR_ANY");
	if (listen(listenfd, 128)){
		perror("The following error occurred, listen: ");
		exit(1);
	}

	p_local_sockaddr(listenfd, "after listen listenfd has same address,port");

	while(true){
		struct sockaddr_in client_addr;
		unsigned int len = sizeof(client_addr);
		bzero(&client_addr, sizeof(client_addr));
		int connfd = accept(listenfd, (struct sockaddr*) &client_addr, &len);
		p_local_sockaddr(connfd, "after accept connfd is a clone of listenfd local IP/port");
		p_local_sockaddr(connfd, "after accept/connect listenfd has local IP");

		if (connfd == -1){
			perror("The following error occurred, accept: ");
			if (errno == ECONNABORTED){
				//continue;
			}
			else{
				exit(1);
			}
		}

		char buff[MAXLEN];
		if (inet_ntop(AF_INET, &(client_addr.sin_addr), buff, sizeof(buff)) != NULL){
			std::cout << "client IP " << buff << " port " << ntohs(client_addr.sin_port) << std::endl;
		}
		else{
			perror("The following error occurred, inet_ntop: ");
		}

		struct pollfd pollfd;
		pollfd.fd = connfd;
		pollfd.events = POLLIN | POLLOUT | POLLHUP;
		size_t n;
		char buf[MAXLEN];
		int cnt=5;
		while(true && cnt)
		{
			poll(&pollfd, 1, -1);
			check_flag(pollfd.revents);
			n = read(connfd, buf, MAXLEN);
			if (n==0) {
				close(connfd);
				std::cerr << "connection closed" << std::endl;
				break;
			}
			sleep(5);
            //writen(connfd, buf, n);
			write(connfd, buf, n);
			write(connfd, buf, n);

            std::cerr << "str_echo n=" << n << " " << std::string(buf, n) << std::endl;
			std::cerr << "cnt " << --cnt << std::endl;
		}
		//str_echo(connfd);
	 }
}

