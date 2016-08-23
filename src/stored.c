/* vim: set tabstop=2 expandtab: */
#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include "util/logger.h"
#include "util/demonizer.h"
#include "mtab_check_trigger.h"
#include "mtab_check.h"
#include "srv/srv.h"

int main()
{
    ST_add_sigint_hook(&ST_break_checks_loop);
    ST_add_sigint_hook(&ST_stop_server);
    ST_demonize();
    ST_start_server();
    ST_msg("daemon started.", ST_MSG_NOTICE);
    ST_checks_loop(&ST_check_mtab);
    ST_stop_server();
    ST_msg("daemon terminated.", ST_MSG_NOTICE);
    return EXIT_SUCCESS;
}