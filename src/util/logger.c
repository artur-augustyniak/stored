#include <config.h>
#include "logger.h"
#include <syslog.h>
#include <stdio.h>

void open_log(const char *name)
{
     openlog(name, LOG_PID, LOG_DAEMON);

}

void close_log()
{
    closelog();
}

void put_notice(char* msg)
{
#ifdef IS_DAEMON
    syslog(LOG_NOTICE, msg);
#else
    printf("DEBUG syslog msg. PRIO: %i. MSG: %s\n", LOG_NOTICE, msg);
#endif
}