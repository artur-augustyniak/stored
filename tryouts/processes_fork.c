#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main() 
{
	printf("START!\n");
	int pid = fork();
	if(pid==0)
	{
		printf("Jestem dzieckiem.\n");
		printf("Moj PID to: %d\n",getpid());
		printf("PID rodzica to: %d\n",getppid());
	}
	else
	{
		sleep(2);
		printf("Jestem rodzicem.\n");
		printf("Moj PID to: %d\n",getpid());
		printf("PID rodzica to: %d\n",getppid());
		printf("PID dziecka to: %d\n",pid);
	}
	return 0;
}
