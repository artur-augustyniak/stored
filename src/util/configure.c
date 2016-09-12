#include <stdlib.h>
#include <unistd.h>
#include <libconfig.h>
#include "mtab_check_trigger.h"
#include "mtab_check.h"
#include "srv/srv.h"
#include "configure.h"

static config_t cfg;


static int cfg_err(const char* section, config_t *cfg)
{
    fprintf(stderr,
        "no proper %s section in %s\n",
        section,
        config_error_file(cfg)
    );
    config_destroy(cfg);
    return(EXIT_FAILURE);
}

void ST_init_config(const char *path)
{
    cfg_file_path = path;
    config_init(&cfg);
}


int ST_read_conf(void)
{
    config_setting_t *core_group;
    config_setting_t *server_group;
    if(! config_read_file(&cfg, cfg_file_path))
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
        if(!(config_setting_lookup_int(core_group, "check_interval_ms", &curr_config.timeout)
            && config_setting_lookup_int(core_group, "free_percent_notice", &curr_config.notice_level)
            && config_setting_lookup_int(core_group, "free_percent_warn", &curr_config.warn_level)
            && config_setting_lookup_int(core_group, "free_percent_crit", &curr_config.crit_level)
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

        if(!(config_setting_lookup_bool(server_group, "enabled", &curr_config.server_enabled)
            && config_setting_lookup_int(server_group, "port", &curr_config.server_port)
            && config_setting_lookup_string(server_group, "bind_addr", (const char **)&curr_config.bind_address)
        ))
        {
            return cfg_err("server", &cfg);
        }
    }
    else
    {
        return cfg_err("server", &cfg);
    }
    return EXIT_SUCCESS;
}

void ST_destroy_config()
{
    config_destroy(&cfg);
}

