#include <unistd.h>
#include <iostream>
#include <string>
#include <algorithm>

#define MAXLEN 100

ssize_t						/* Read "n" bytes from a descriptor. */
readn(int fd, char *vptr, size_t n)
{
    size_t	nleft;
    ssize_t	nread;
    char	*ptr;

    ptr = vptr;
    nleft = n;
    while (nleft > 0) {
        if ( (nread = read(fd, ptr, nleft)) < 0) {
            if (errno == EINTR)
                nread = 0;		/* and call read() again */
            else
                return(-1);
        } else if (nread == 0)
            break;				/* EOF */

        nleft -= nread;
        ptr   += nread;
    }
    return(n - nleft);		/* return >= 0 */
}

ssize_t						/* Write "n" bytes to a descriptor. */
writen(int fd, const void *vptr, size_t n)
{
	size_t		nleft;
	ssize_t		nwritten;
	const char	*ptr;

	ptr = (const char*) vptr;
	nleft = n;
	while (nleft > 0) {
		if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
			if (nwritten < 0 && errno == EINTR)
				nwritten = 0;		/* and call write() again */
			else
				return(-1);			/* error */
		}

		nleft -= nwritten;
		ptr   += nwritten;
	}
	return(n);
}

void str_echo(int sockfd){
	size_t n;
	char buf[MAXLEN];
	while(true) {
        n = read(sockfd, buf, MAXLEN);
        if ( n > 0) {
            writen(sockfd, buf, n);
            std::cerr << "str_echo n=" << n << std::string(buf, n) << std::endl;
        }
        else if ( n < 0 && errno == EINTR){
            continue;
        }
        else if (n == 0){
            std::cerr << "str_echo n==0" << std::endl;
            return;
        }
        else{
            perror("error str_echo");
        }
    }
}

void
str_cli(FILE *fp, int sockfd)
{
    char    sendline[MAXLEN], recvline[MAXLEN];

    while (fgets(sendline, MAXLEN, fp) != NULL) {
        writen(sockfd, sendline, strlen(sendline));
        int n = read(sockfd, recvline, MAXLEN);
        if(n>0) {
            recvline[std::min(n, MAXLEN-1)]=0;
            fputs(recvline, stdout);
            std::cerr << std::endl << "=========" << std::endl;
            std::cerr << "n=" << n << std::endl;
            std::cerr << (int)recvline[0] << std::endl;
        }
        else{
            perror("str_cli error: ");
            return;
        }
    }
    std::cerr << "str_cli fgets returns NULL" << std::endl;
}

