/* vim: set tabstop=2 expandtab: */
#include <config.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdio.h>
#include <signal.h>
#include <stdbool.h>
#include <string.h>
#include "util/configure.h"
#include "util/logger.h"
#include "util/demonizer.h"
#include "mtab_check.h"
#include "mtab_check_trigger.h"

static bool active = true;
static ST_conf conf;
static char *conf_path;

static void reload (void)
{
    ST_logger_msg("daemon reloading.", ST_MSG_NOTICE);
    ST_break_checks_loop();
}

static void stop(void)
{
    ST_logger_msg("daemon terminating.", ST_MSG_NOTICE);
    active = false;
    ST_break_checks_loop();
}


int main(int argc, char *argv[])
{
    int opt;
    while ((opt = getopt(argc, argv, "f:")) != -1)
    {
        switch (opt)
        {
            case 'f':
            conf_path = strdup(optarg);
            if(conf_path)
            {
                ST_logger_init(PACKAGE_NAME, ST_STDOUT);
                ST_init_demonizer(ST_NOTIFY);
                ST_add_signal_hook(SIGINT, &stop);
                ST_add_signal_hook(SIGHUP, &reload);
                ST_demonize();
                ST_logger_msg("daemon started.", ST_MSG_NOTICE);
                while(active)
                {
                    conf = ST_config(optarg);
                    if(conf)
                    {
                        ST_init_checks_loop(conf);
                        ST_init_check_mtab(conf);
                        ST_checks_loop(&ST_check_mtab, conf->timeout);
                        ST_destroy_check_mtab();
                    }
                    else
                    {
                        active = false;
                        ST_logger_msg("configuration error.", ST_MSG_CRIT);
                    }
                    ST_destroy_config();
                }
                ST_logger_msg("daemon terminated.", ST_MSG_NOTICE);
                ST_destroy_demonizer();
                ST_logger_destroy();
                free(conf_path);
                exit(EXIT_SUCCESS);
            }
            break;
        }
    }
    fprintf(stderr, "Usage: %s [-f <path/config.cfg>]\n", argv[0]);
    return EXIT_SUCCESS;
}