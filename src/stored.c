/* vim: set tabstop=2 expandtab: */
#include <config.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include "util/logger.h"
#include "util/demonizer.h"
#include "util/config.h"
#include "mtab_check_trigger.h"
#include "mtab_check.h"
#include "srv/srv.h"

int main(int argc, char *argv[])
{
    config_t cfg;
    config_init(&cfg);
    int read_cfg_status;

    int opt;
    while ((opt = getopt(argc, argv, "bhf:")) != -1)
    {
        switch (opt)
        {
            case 'b':
                ST_op_mode = ST_FORKING;
                ST_sink_type = ST_SYSLOG;
                break;
            case 'f':
                read_cfg_status = read_conf(&cfg);
                if(! EXIT_SUCCESS == read_cfg_status)
                {
                    return read_cfg_status;
                }
                break;
            case 'h':
                fprintf(stderr, "Usage: %s [-bh] [-f <path/config.cfg>]\n", argv[0]);
                exit(EXIT_SUCCESS);
                break;
            default:
                ST_op_mode = ST_NOTIFY;
                ST_sink_type = ST_STDOUT;
        }
    }

    ST_add_sigint_hook(&ST_break_checks_loop);
    if(ST_enabled)
        ST_add_sigint_hook(&ST_stop_server);

    ST_demonize();

//    if(ST_enabled)
//        ST_start_server();
    ST_msg("daemon started.", ST_MSG_NOTICE);

    ST_checks_loop(&ST_check_mtab);

//    if(ST_enabled)
//        ST_stop_server();
    config_destroy(&cfg);
    ST_msg("daemon terminated.", ST_MSG_NOTICE);
    return EXIT_SUCCESS;
}