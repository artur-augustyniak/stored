#include <config.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "logger.h"
#include "mtab_check_trigger.h"
void skeleton_daemon()
{
#ifdef IS_DAEMON
    pid_t pid;

    /* Fork off the parent process */
    pid = fork();

    /* An error occurred */
    if (pid < 0)
        exit(EXIT_FAILURE);

    /* Success: Let the parent terminate */
    if (pid > 0)
        exit(EXIT_SUCCESS);

    /* On success: The child process becomes session leader */
    if (setsid() < 0)
        exit(EXIT_FAILURE);

    /* Catch, ignore and handle signals */
    //TODO: Implement a working signal handler */
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);

    /* Fork off for the second time*/
    pid = fork();

    /* An error occurred */
    if (pid < 0)
        exit(EXIT_FAILURE);

    /* Success: Let the parent terminate */
    if (pid > 0)
        exit(EXIT_SUCCESS);

    /* Set new file permissions */
    umask(0);
    /* Change the working directory to the root directory */
    /* or another appropriated directory */
    chdir("/tmp");

    /* Close all open file descriptors 65536 ?*/
    int x;
    for (x = sysconf(_SC_OPEN_MAX); x>0; x--)
    {
        close(x);
    }
    /* Open the log file */
    open_log(PACKAGE_NAME);
#else
    put_notice("Debug mode.");
#endif
}

void sigint_handler(int sig) {
	break_checks_loop();
}
