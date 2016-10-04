/* vim: set tabstop=2 expandtab: */
#include <stdlib.h>
#include <unistd.h>
#include <libconfig.h>
#include "configure.h"

static config_t cfg;

ST_CONFIG ST_new_config(const char *conf_file_path)
{
    ST_CONFIG c;
    int error_line = 0;
    const char *error_msg;
    config_setting_t *core_group;
    config_setting_t *server_group;

    config_init(&cfg);

    c = (ST_CONFIG) malloc(sizeof(ST_CONFIG_STRUCT));
    if(!c)
    {
        ST_abort(
            __FILE__,
            __LINE__,
            "OOM error. Exiting!"
        );
        config_destroy(&cfg);
    }

    if(!config_read_file(&cfg, conf_file_path))
    {
        error_line = config_error_line(&cfg);
        error_msg = config_error_text(&cfg);
        ST_destroy_config(c);
        ST_abort(conf_file_path, error_line, error_msg);
    }

    core_group = config_lookup(&cfg, "stored.core");
    if( NULL == core_group || !(
               config_setting_lookup_int(core_group, "check_interval_ms", &c->interval)
            && config_setting_lookup_int(core_group, "free_percent_notice", &c->notice_percent)
            && config_setting_lookup_int(core_group, "free_percent_warn", &c->warn_percent)
            && config_setting_lookup_int(core_group, "free_percent_crit", &c->crit_percent)
    ))
    {
        ST_destroy_config(c);
        ST_abort(conf_file_path, error_line, "stored.core value error");
    }

    server_group = config_lookup(&cfg, "stored.server");
    if( NULL == server_group || !(
                config_setting_lookup_bool(server_group, "enabled", &c->http_enabled)
            && config_setting_lookup_int(server_group, "port", &c->http_port)
            && config_setting_lookup_string(server_group, "bind_addr", &c->http_bind_address)
    ))
    {
        ST_destroy_config(c);
        ST_abort(conf_file_path, error_line, "stored.server value error");
    }
    return c;
}

void ST_destroy_config(ST_CONFIG c)
{
    config_destroy(&cfg);
    free(c);
}