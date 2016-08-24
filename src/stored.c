/* vim: set tabstop=2 expandtab: */
#include <config.h>
#include <stdlib.h>

#include <stdbool.h>
//#include <stdio.h>
//#include <stdlib.h>
#include <unistd.h>

#include "util/logger.h"
#include "util/demonizer.h"
#include "mtab_check_trigger.h"
#include "mtab_check.h"
#include "srv/srv.h"

int main(int argc, char *argv[])
{
    int opt;
    while ((opt = getopt(argc, argv, "bh")) != -1) {
        switch (opt) {
        case 'b':
            ST_op_mode = ST_FORKING;
            ST_sink_type = ST_SYSLOG;
            break;
        case 'h':
            fprintf(stderr, "Usage: %s [-bh]\n", argv[0]);
            exit(EXIT_SUCCESS);
            break;
        default:
            ST_op_mode = ST_NOTIFY;
            ST_sink_type = ST_STDOUT;
        }
    }
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