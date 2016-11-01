/* vim: set tabstop=2 expandtab: */
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <systemd/sd-daemon.h>
#include "demonizer.h"
#include "logger.h"

#define HOOKS_NUM  4

static ST_OP_MODE op_mode = -1;
static bool demonized = false;
static ST_signal_hook sigint_hooks[HOOKS_NUM] = {NULL};
static ST_signal_hook sighup_hooks[HOOKS_NUM] = {NULL};
static int int_hook_idx = 0;
static int hup_hook_idx = 0;

void ST_init_demonizer(ST_OP_MODE mode) {
    op_mode = mode;
}

static void sighup_handler(int sig) {
    for (int i = 0; i < HOOKS_NUM; i++) {
        if (NULL != sighup_hooks[i]) {
            sighup_hooks[i]();
        }
    }
}

static void sigint_handler(int sig) {
    for (int i = 0; i < HOOKS_NUM; i++) {
        if (NULL != sigint_hooks[i]) {
            sigint_hooks[i]();
        }
    }
}

int ST_add_signal_hook(int sig, void (*signal_hook)(void)) {
    switch (sig) {
        case SIGINT:
            if (int_hook_idx < HOOKS_NUM) {
                sigint_hooks[int_hook_idx++] = signal_hook;
                return 0;
            } else {
                return 1;
            }
            break;
        case SIGHUP:
            if (hup_hook_idx < HOOKS_NUM) {
                sighup_hooks[hup_hook_idx++] = signal_hook;
                return 0;
            } else {
                return 1;
            }
            break;
        default:
            ST_logger_msg("Unsupported signal type", ST_MSG_ERROR);
    }
    return 1;
}

void ST_demonize(void) {
    if (demonized) {
        return;
    }

    signal(SIGINT, &sigint_handler);
    signal(SIGHUP, &sighup_handler);
    signal(SIGCHLD, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);

    bool _sd_booted = sd_booted();
    pid_t pid;

    switch (op_mode) {
        case ST_NOTIFY:
            if (_sd_booted) {
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
            for (x = sysconf(_SC_OPEN_MAX); x > 0; x--) {
                close(x);
            }
            break;
        default:
            ST_logger_msg("Unsupported operation type", ST_MSG_ERROR);
    }
    demonized = true;
}

void ST_destroy_demonizer(void) {
    //unsupported
}

