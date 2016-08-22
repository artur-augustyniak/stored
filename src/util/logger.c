#include <config.h>
#include <syslog.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "logger.h"

#define MSG_FMT "<%i> %s\n"

ST_SINK ST_sink_type = ST_STDOUT;

static bool log_active = false;

void ST_msg(char* msg, int type)
{
    switch (ST_sink_type) {
        case ST_STDOUT:
            fprintf(stdout, MSG_FMT, type, msg);
            break;
        case ST_SYSLOG:
            if(!log_active)
            {
                atexit(&closelog);
                openlog(PACKAGE_NAME, LOG_PID, LOG_DAEMON);
                log_active = true;
            }
            syslog(type, msg);
            break;
        default:
            fprintf(stdout, "Unsupported logger\n");
    }
}