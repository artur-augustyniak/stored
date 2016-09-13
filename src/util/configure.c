/* vim: set tabstop=2 expandtab: */
#include <stdlib.h>
#include <unistd.h>
#include <libconfig.h>
#include "configure.h"

static ST_conf conf_handle;
static config_t cfg;

static int cfg_err(const char* section)
{
    fprintf(stderr,
        "no proper %s section in %s\n",
        section,
        config_error_file(&cfg)
    );
    config_destroy(&cfg);
    return(EXIT_FAILURE);
}

static int read_conf(const char *fpath)
{

    config_setting_t *core_group;
    config_setting_t *server_group;
    if(! config_read_file(&cfg, fpath))
    {
        fprintf(stderr,
            "%s:%d - %s\n",
            config_error_file(&cfg),
            config_error_line(&cfg),
            config_error_text(&cfg)
        );
        //config_destroy(&cfg);
        return(EXIT_FAILURE);
    }

    core_group = config_lookup(&cfg, "stored.core");
    if(core_group != NULL)
    {
        if(!(config_setting_lookup_int(core_group, "check_interval_ms", &conf_handle->timeout)
            && config_setting_lookup_int(core_group, "free_percent_notice", &conf_handle->notice_level)
            && config_setting_lookup_int(core_group, "free_percent_warn", &conf_handle->warn_level)
            && config_setting_lookup_int(core_group, "free_percent_crit", &conf_handle->crit_level)
        ))
        {
            return cfg_err("core");
        }
    }
    else
    {
        return cfg_err("core");
    }
    const char *str;
    server_group = config_lookup(&cfg, "stored.server");
    if(server_group != NULL)
    {

        if(!(config_setting_lookup_bool(server_group, "enabled", &conf_handle->server_enabled)
            && config_setting_lookup_int(server_group, "port", &conf_handle->server_port)
            && config_setting_lookup_string(server_group, "bind_addr", &conf_handle->bind_address)
        ))
        {
            return cfg_err("server");
        }
    }
    else
    {
        return cfg_err("server");
    }
    return EXIT_SUCCESS;
}

ST_conf ST_config(const char *fpath)
{
    config_init(&cfg);
    conf_handle = (ST_conf) malloc(sizeof(ST_CONF));

    if(EXIT_SUCCESS == read_conf(fpath))
    {
        return conf_handle;
    }
    return NULL;
}

void ST_destroy(ST_conf c)
{
    //config_destroy(&cfg);
    free(c);
}