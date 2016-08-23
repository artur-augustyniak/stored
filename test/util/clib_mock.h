#include <stdio.h>

unsigned int openlog_call_counter = 0;
unsigned int closelog_call_counter = 0;
unsigned int atexit_call_counter = 0;
unsigned int syslog_call_counter = 0;
void openlog(const char *ident, int option, int facility)
{
    openlog_call_counter++;
}

void closelog(void)
{
    closelog_call_counter++;
}

int atexit(void (*f)(void))
{
    f();
    atexit_call_counter++;
    return 0;
}

void syslog(int priority, const char *format, ...){
    syslog_call_counter++;
}