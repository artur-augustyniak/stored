/* vim: set tabstop=2 expandtab: */
#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "util/logger.h"
#include "util/demonizer.h"
#include "mtab_check_trigger.h"
#include "mtab_check.h"


const char NAME[] = "stored";

int main()
{
    signal(SIGINT, &sigint_handler);
    atexit(&destroy_mtab);
    skeleton_daemon();
    put_notice("stored daemon started.");
    init_checks_loop();
    init_mtab();
    checks_loop(&check_mtab);
    put_notice("stored daemon terminated.");

#ifdef IS_DAEMON
    close_log();
#endif

    return EXIT_SUCCESS;
}