#include <config.h>
#include <syslog.h>
#include <stdio.h>
#include <stdbool.h>
#include "logger.h"

static bool is_active = false;

static void open_log(void)
{
    atexit(&closelog);
    openlog(PACKAGE_NAME, LOG_PID, LOG_DAEMON);
}


void msg(char* msg, int type)
{
    if(!is_active)
    {
        open_log();
        is_active = true;
    }
    printf("TYPE: %i - %s\n", type, msg);
}

//## MESSS

//static void put_real_msg(char* msg, int severity)
//{
//#ifdef IS_DAEMON
//    syslog(LOG_NOTICE, msg);
//#else
//    printf("DEBUG syslog msg. PRIO: %i. MSG: %s\n", LOG_NOTICE, msg);
//#endif
//    #ifdef IS_DAEMON
//        atexit(&close_log);
//    #endif
//
//}
//void open_log(const char *name)
//{
//     openlog(name, LOG_PID, LOG_DAEMON);
//}
//
//void close_log()
//{
//    closelog();
//}
//
//void put_notice(char* msg)
//{
//    put_real_msg(msg, LOG_NOTICE);
//}
//
//void put_warn(char* msg){
//    put_real_msg(msg, LOG_WARNING);
//}
//
//void put_crit(char* msg){
//    put_real_msg(msg, LOG_CRIT);
//}
//
//void put_error(char* msg){
//    put_real_msg(msg, LOG_ERR);
//}