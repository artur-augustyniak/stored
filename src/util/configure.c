#include <stdlib.h>
#include <unistd.h>
#include "configure.h"
#include "mtab_check_trigger.h"
#include "mtab_check.h"
#include "srv/srv.h"

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

int read_conf(config_t *cfg)
{
    config_setting_t *core_group;
    config_setting_t *server_group;
    if(! config_read_file(cfg, optarg))
    {
        fprintf(stderr,
            "%s:%d - %s\n",
            config_error_file(cfg),
            config_error_line(cfg),
            config_error_text(cfg)
        );
        config_destroy(cfg);
        return(EXIT_FAILURE);
    }

    core_group = config_lookup(cfg, "stored.core");
    if(core_group != NULL)
    {
        if(!(config_setting_lookup_int(core_group, "check_interval_ms", &ST_timeout)
            && config_setting_lookup_int(core_group, "free_percent_notice", &ST_notice_level)
            && config_setting_lookup_int(core_group, "free_percent_warn", &ST_warn_level)
            && config_setting_lookup_int(core_group, "free_percent_crit", &ST_crit_level)
        ))
        {
            return cfg_err("core", cfg);
        }
    }
    else
    {
        return cfg_err("core", cfg);
    }

    server_group = config_lookup(cfg, "stored.server");
    if(server_group != NULL)
    {
        if(!(config_setting_lookup_bool(server_group, "enabled", &ST_enabled)
            && config_setting_lookup_int(server_group, "port", &ST_port)
            && config_setting_lookup_string(server_group, "bind_addr", &ST_bind_addr)
        ))
        {
            return cfg_err("server", cfg);
        }
    }
    else
    {
        return cfg_err("server", cfg);
    }
    return EXIT_SUCCESS;
}