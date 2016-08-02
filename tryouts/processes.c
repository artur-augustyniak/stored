#include<sys/types.h>
#include<unistd.h>
#include<stdio.h>

int main()
{

	printf("START!\n");
	fork();
	printf("Moj PID to %d\n",getpid());
	printf("PID mojego przodka to: %d\n",getppid());
	printf("STOP!\n");
	int s;
	wait(&s);
	return 0;
}
