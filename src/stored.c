/* vim: set tabstop=2 expandtab: */
#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "util/logger.h"
#include "util/demonizer.h"
#include "mtab_check_trigger.h"
#include "mtab_check.h"




int main()
{
    signal(SIGINT, &sigint_handler);
    atexit(&destroy_mtab);
    atexit(&destory_current_notices);
    #ifdef IS_DAEMON
        atexit(&close_log);
    #endif
    skeleton_daemon();
    put_notice("stored daemon started.");
    init_checks_loop();
    init_mtab();
    checks_loop(&check_mtab);
    put_notice("stored daemon terminated.");
    return EXIT_SUCCESS;
}