/* vim: set tabstop=2 expandtab: */
#include <config.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <libconfig.h>
#include "util/logger.h"
#include "util/demonizer.h"
#include "mtab_check_trigger.h"
#include "mtab_check.h"
#include "srv/srv.h"




int cfg_err(const char* section, config_t *cfg)
{
    fprintf(stderr,
        "no proper %s section in %s\n",
        section,
        config_error_file(cfg)
    );
    config_destroy(cfg);
    return(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    config_t cfg;
    config_setting_t *core_group;
    config_setting_t *server_group;
    config_init(&cfg);

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
                if(! config_read_file(&cfg, optarg))
                {
                        fprintf(stderr,
                            "%s:%d - %s\n",
                            config_error_file(&cfg),
                            config_error_line(&cfg),
                            config_error_text(&cfg)
                        );
                        config_destroy(&cfg);
                        return(EXIT_FAILURE);
                    }

                    core_group = config_lookup(&cfg, "stored.core");
                    if(core_group != NULL)
                    {
                        if(!(config_setting_lookup_int(core_group, "check_interval_ms", &ST_timeout)
                            && config_setting_lookup_int(core_group, "free_percent_notice", &ST_notice_level)
                            && config_setting_lookup_int(core_group, "free_percent_warn", &ST_warn_level)
                            && config_setting_lookup_int(core_group, "free_percent_crit", &ST_crit_level)
                         ))
                         {
                                return cfg_err("core", &cfg);
                         }
                    }
                    else
                    {
                        return cfg_err("core", &cfg);
                    }


                    server_group = config_lookup(&cfg, "stored.server");
                    if(server_group != NULL)
                    {
                        if(!(config_setting_lookup_bool(server_group, "enabled", &ST_enabled)
                        && config_setting_lookup_int(server_group, "port", &ST_port)
                        && config_setting_lookup_string(server_group, "bind_addr", &ST_bind_addr)
                        ))
                        {
                            return cfg_err("server", &cfg);
                        }
                    }
                    else
                    {
                        return cfg_err("server", &cfg);
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

    if(ST_enabled)
        ST_start_server();
    ST_msg("daemon started.", ST_MSG_NOTICE);

    ST_checks_loop(&ST_check_mtab);

    if(ST_enabled)
        ST_stop_server();
    config_destroy(&cfg);
    ST_msg("daemon terminated.", ST_MSG_NOTICE);
    return EXIT_SUCCESS;
}