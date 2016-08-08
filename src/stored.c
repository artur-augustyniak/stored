/* vim: set tabstop=2 expandtab: */
#include <config/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>

#include "mtab_check_trigger.h"
#include "mtab_check.h"

static const char NAME[] = "stored";
/**
* gcc -Wall -pedantic -std=c99 stored.c
*/
static void skeleton_daemon()
{
#ifdef DAEMON
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
#else
    printf("Debug mode\n");
#endif
    /* Open the log file */
    openlog (NAME, LOG_PID, LOG_DAEMON);



}


int main()
{
    skeleton_daemon();
    syslog (LOG_NOTICE, "First daemon started.");
    trigger_check(&check);
    syslog (LOG_NOTICE, "First daemon terminated.");
    closelog();
    return EXIT_SUCCESS;
}

