#include <poll.h>
#include <unistd.h>
#include <sys/timerfd.h>
#include <iostream>
#include <time.h>

int f(int n)
{
	if(n==1) return 1;
	if(n==0) return 0;
	return f(n-1)+f(n-2);
}

int main(int argc, char *argv[])
{
	int tfd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC);
	struct itimerspec new_value;
	new_value.it_value.tv_sec = 5;
	new_value.it_interval.tv_sec = 3;
	timerfd_settime(tfd, 0, &new_value, NULL);
	
	struct pollfd *pollfd;
	pollfd = new struct pollfd();
	pollfd->fd = tfd;
	pollfd->events = POLLIN;
	uint64_t numexp = 0;
	for (int i=0; i<5; ++i)
	{
		if (i==2) sleep(10);

		std::cout << "before poll" << std::endl;
		/*
		poll 
		http://man7.org/linux/man-pages/man2/timerfd_create.2.html
		*/
		int readyfd = poll(pollfd, 1, -1);
		std::cout << "after poll readyfd " << readyfd << std::endl;
		std::cout << "before read" << std::endl;
		/*
		read
		*/
		ssize_t s = read(tfd, &numexp, sizeof(numexp));
		std::cout << "after read s=" << s << " after read numexp=" << numexp << std::endl;

		std::cout << f(40) << std::endl;
		
	}
}
