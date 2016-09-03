/* vim: set tabstop=2 expandtab: */
#include<stdio.h>
#include <signal.h>

static void sighup_handler(int sig)
{
    printf("sighup_handler\n");
}


void main(void)
{
    signal(SIGINT, &sigint_handler);
    signal(SIGHUP, &sighup_handler);
    for(;;){
        sleep(10);
    }
    printf("blah");
}
