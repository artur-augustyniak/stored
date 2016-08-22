/* vim: set tabstop=2 expandtab: */
#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include "util/logger.h"
#include "util/demonizer.h"
#include "mtab_check_trigger.h"
#include "mtab_check.h"
#include "srv/srv.h"

int main()
{
    signal(SIGINT, &sigint_handler);
    atexit(&destroy_mtab);
    skeleton_daemon();
    ST_msg("stored daemon started.", ST_MSG_NOTICE);
    init_checks_loop();
    init_mtab();

    pthread_t srv_thread;

    init_server();

    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGPIPE);
    pthread_sigmask(SIG_BLOCK, &set, NULL);
    pthread_create(&srv_thread, NULL, &run_server, &mxq);

    checks_loop(&check_mtab);

    pthread_join(srv_thread, NULL);
    ST_msg("stored daemon terminated.", ST_MSG_NOTICE);
    return EXIT_SUCCESS;
}