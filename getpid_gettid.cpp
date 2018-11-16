#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>

using namespace std;

pid_t gettid()
{
     return syscall(SYS_gettid); /*glibc does not implement gettid, need kernel API*/
}
 
int main()
{
    int pid;
	
    pid = fork();
    if (pid == 0)
    {
        cout << "\nChild process:" << endl;
		cout << "gettid : " << gettid() << endl;
		cout << "getpid : " << getpid() << endl;
		cout << "pid = fork() " << pid << endl;
        cout << "getppid : " << getppid() << endl;
    }
	else
	{
        cout << "\nParent process:" << endl;
        cout << "gettid : " << gettid() << endl;
        cout << "getpid : " << getpid() << endl;
        cout << "pid = fork() " << pid << endl;
        cout << "getppid : " << getppid() << endl;
		/*
		sleep(1) is to make sure parent process still exists.
		Otherwise, getppid() in child process will not return valid descriptor
		*/
		sleep(1);
	}
    
	return 0;
}
