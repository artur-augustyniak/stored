#include<stdio.h>
#include<string.h>
#include<signal.h>
#include <sys/types.h>
#include <unistd.h>


void handler(int sig) {
	printf("A ja nie chce!!!\n");
};


int main()
{
	printf("%s\n", strsignal(SIGINT));
	printf("%s\n", strsignal(SIGSEGV));
	printf("%s\n", strsignal(SIGBUS));
	printf("moj pidi %d\n", getpid());

	// example kill kill -TERM PID	
	signal(SIGTERM,handler);
	while(1)
	{
		;
	}

	return 0;
};


