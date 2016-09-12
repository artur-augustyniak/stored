/* vim: set tabstop=2 expandtab: */
#include <config.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include "util/logger.h"
#include "util/demonizer.h"
#include "util/configure.h"
#include "mtab_check_trigger.h"
#include "mtab_check.h"
#include "srv/srv.h"
#include <getopt.h>

void blah(void)
{
    int read_cfg_status;
    printf("sighup\n");
    ST_break_checks_loop();
    //ST_init_config(cfg_file_path);



    read_cfg_status = ST_read_conf();
    if(! EXIT_SUCCESS == read_cfg_status)
    {
        return;
    }
    ST_destroy_config();

    ST_logger_msg("daemon restarted.", ST_MSG_NOTICE);
    ST_checks_loop(&ST_check_mtab);
}

int main(int argc, char *argv[])
{
    int read_cfg_status;
    int opt;
    while ((opt = getopt(argc, argv, "bhf:")) != -1)
    {
        switch (opt)
        {
            case 'b': //background forking
                ST_logger_init(PACKAGE_NAME, ST_SYSLOG);
                ST_init_demonizer(ST_FORKING);
                break;
            case 'f': // config file
                ST_init_config(optarg);
                ST_logger_init(PACKAGE_NAME, ST_STDOUT);
                ST_init_demonizer(ST_NOTIFY);
                read_cfg_status = ST_read_conf();
                if(! EXIT_SUCCESS == read_cfg_status)
                {
                    return read_cfg_status;
                }
                ST_destroy_config();
                break;
            case 'h':
                fprintf(stderr, "Usage: %s [-bh] [-f <path/config.cfg>]\n", argv[0]);
                exit(EXIT_SUCCESS);
                break;
            default:
                ST_logger_init(PACKAGE_NAME, ST_STDOUT);
                ST_init_demonizer(ST_NOTIFY);
        }
    }


    ST_add_signal_hook(SIGINT, &ST_break_checks_loop);
    ST_add_signal_hook(SIGINT, &ST_stop_server);
    ST_add_signal_hook(SIGHUP, &blah);
    ST_demonize();
    ST_start_server();
    ST_logger_msg("daemon started.", ST_MSG_NOTICE);

    ST_checks_loop(&ST_check_mtab);

    ST_stop_server();
    ST_logger_msg("daemon terminated.", ST_MSG_NOTICE);

    ST_destroy_demonizer();
    ST_logger_destroy();
    return EXIT_SUCCESS;
}