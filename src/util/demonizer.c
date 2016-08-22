#include <config.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "logger.h"
#include "mtab_check_trigger.h"
#include "srv/srv.h"

void skeleton_daemon()
{
#ifdef IS_DAEMON
    pid_t pid;
    pid = fork();
    if (pid < 0)
        exit(EXIT_FAILURE);
    if (pid > 0)
        exit(EXIT_SUCCESS);
    if (setsid() < 0)
        exit(EXIT_FAILURE);
    /* Catch, ignore and handle signals */
    //TODO: Implement a working signal handler */
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);
    pid = fork();
    if (pid < 0)
        exit(EXIT_FAILURE);
    if (pid > 0)
        exit(EXIT_SUCCESS);
    umask(066);
    chdir("/tmp");
    int x;
    for (x = sysconf(_SC_OPEN_MAX); x>0; x--)
    {
        close(x);
    }
#else
    ST_msg("Debug mode.", ST_MSG_NOTICE);
#endif
}

void sigint_handler(int sig) {
	break_checks_loop();
	stop_server();
}

