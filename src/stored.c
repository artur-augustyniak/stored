/* vim: set tabstop=2 expandtab: */
#include <stdlib.h>
#include <getopt.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include "util/common.h"
#include "util/configure.h"
#include "util/logger.h"
#include "util/demonizer.h"
#include "util/srv.h"
#include "mtab_checker.h"
#include "mtab_check_loop.h"
#include "stored_config.h"


static ST_CONFIG core_config = NULL;
static ST_MTAB_ENTRIES entries = NULL;
static ST_SRV_BUFF srv_buff = NULL;
static char *conf_path;

static void reload(void) {
    ST_logger_msg("daemon reloading.", ST_MSG_NOTICE);
    ST_reload_config(core_config);
    ST_restart_srv(srv_buff);
}

static void stop(void) {
    ST_logger_msg("daemon terminating.", ST_MSG_NOTICE);
    ST_break_check_loop();
}

int main(int argc, char *argv[]) {
    int opt;
    while ((opt = getopt(argc, argv, "f:")) != -1) {
        switch (opt) {
            case 'f':
                conf_path = strdup(optarg);
                if (conf_path) {
                    ST_logger_init(PROJECT_NAME, ST_STDOUT);
                    ST_init_demonizer(ST_NOTIFY);

                    ST_add_signal_hook(SIGINT, &stop);
                    ST_add_signal_hook(SIGHUP, &reload);
                    ST_demonize();
                    ST_logger_msg("daemon started.", ST_MSG_NOTICE);

                    core_config = ST_new_config(conf_path);
                    entries = ST_init_mtab_checker(core_config);
                    srv_buff = ST_init_srv(core_config);
                    srv_buff->data = "{}";
                    ST_start_srv(srv_buff);
                    ST_init_check_loop(core_config, srv_buff, entries);

                    ST_check_loop();

                    ST_stop_srv(srv_buff);
                    ST_destroy_srv(srv_buff);
                    ST_destroy_mtab_checker(entries);

                    ST_destroy_config(core_config);
                    ST_logger_msg("daemon terminated.", ST_MSG_NOTICE);

                    ST_destroy_demonizer();
                    ST_logger_destroy();

                    free(conf_path);
                    exit(EXIT_SUCCESS);
                }
                break;
        }
    }
    fprintf(stderr, "Usage: %s [-f <path/config.cfg>]\n", PROJECT_NAME);
    return EXIT_SUCCESS;
}