#include <config.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <systemd/sd-daemon.h>
#include "demonizer.h"
#include "logger.h"

#define HOOKS_NUM  4

ST_OP_MODE ST_op_mode = ST_NOTIFY;
static ST_exit_hook hooks[HOOKS_NUM] = {NULL};
static int curr_hook = 0;
static bool demonized = false;

static void sigint_handler(int sig)
{
    for(int i = 0; i < HOOKS_NUM; i++)
    {
        if(NULL != hooks[i])
        {
            hooks[i]();
        }
    }
}

int ST_add_sigint_hook(void (*func)(void))
{
    if(curr_hook < HOOKS_NUM){
        hooks[curr_hook++] = func;
        return 0;
    }
    else{
        return 1;
    }
}

void ST_demonize(void)
{
    if(demonized)
    {
        return;
    }
    signal(SIGINT, &sigint_handler);

    /* Catch, ignore and handle signals */
    //TODO: Implement a working signal handler */
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);

    bool _sd_booted = sd_booted();
    pid_t pid;
    switch (ST_op_mode)
    {
        case ST_NOTIFY:
            if(_sd_booted)
            {
                sd_notify(1, "READY=1");
            }
        break;
        case ST_FORKING:
            pid = fork();
            if (pid < 0)
                exit(EXIT_FAILURE);
            if (pid > 0)
                exit(EXIT_SUCCESS);
            if (setsid() < 0)
                exit(EXIT_FAILURE);
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
        break;
        default:
            ST_msg("Unsupported operation type", ST_MSG_ERROR);
    }
    demonized = true;
}



