/* vim: set tabstop=2 expandtab: */
#include <syslog.h>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "logger.h"
#include "sds.h"

#define MSG_FMT "<%i> %s\n"

static sds daemon_name;
static ST_SINK sink_type;

void ST_logger_init(const char* name, ST_SINK type)
{
    sink_type = type;
    daemon_name = sdsnew(name);
    if(ST_SYSLOG == sink_type)
    {
        openlog(daemon_name, LOG_PID, LOG_DAEMON);
    }
}

void ST_logger_msg(char* msg, int type)
{
    if(!daemon_name || !sink_type)
    {
        printf("%d %s\n", sink_type, daemon_name);
        ST_abort(
            __FILE__,
            __LINE__,
            "Logger uninitialized"
        );
    }
    switch (sink_type)
    {
        case ST_STDOUT:
            fprintf(stdout, MSG_FMT, type, msg);
            break;
        case ST_SYSLOG:
            syslog(type, msg);
            break;
        default:
            fprintf(stdout, "Unsupported logger\n");
    }
}

void ST_logger_destroy(void)
{
    sdsfree(daemon_name);
    if(ST_SYSLOG == sink_type)
    {
        closelog();
    }
}