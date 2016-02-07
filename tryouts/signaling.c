#include<stdio.h>
#include<string.h>
#include<signal.h>


int main()
{
	printf("%s\n", strsignal(SIGINT));
	printf("%s\n", strsignal(SIGSEGV));
	printf("%s\n", strsignal(SIGBUS));

	// example kill kill -SIGSEGV PID	
	signal(SIGINT,SIG_IGN);
	signal(SIGTERM,SIG_IGN);
	while(1)
	{
		;
	}

	return 0;
};


