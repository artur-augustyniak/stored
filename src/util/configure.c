/* vim: set tabstop=2 expandtab: */
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <libconfig.h>
#include <string.h>
#include "configure.h"
#include "common.h"

static config_t cfg;
static const char *file_path;

static load_from_file(ST_CONFIG c)
{
    int error_line = 0;
    const char *error_msg;
//    config_destroy(&cfg);
    config_init(&cfg);
    if(!config_read_file(&cfg, file_path))
    {
        error_line = config_error_line(&cfg);
        error_msg = config_error_text(&cfg);
        ST_destroy_config(c);
        ST_abort(file_path, error_line, error_msg);
    }

    config_setting_t *core_group;
    config_setting_t *server_group;

    core_group = config_lookup(&cfg, "stored.core");
    if( NULL == core_group || !(
               config_setting_lookup_int(core_group, "check_interval_ms", &c->interval)
            && config_setting_lookup_int(core_group, "free_percent_notice", &c->notice_percent)
            && config_setting_lookup_int(core_group, "free_percent_warn", &c->warn_percent)
            && config_setting_lookup_int(core_group, "free_percent_crit", &c->crit_percent)
    ))
    {
        ST_destroy_config(c);
        ST_abort(
            __FILE__,
            __LINE__,
            "stored.core value error"
        );
    }

    server_group = config_lookup(&cfg, "stored.server");
    if( NULL == server_group || !(
                config_setting_lookup_bool(server_group, "enabled", &c->http_enabled)
            && config_setting_lookup_int(server_group, "port", &c->http_port)
            && config_setting_lookup_string(server_group, "bind_addr", &c->http_bind_address)
    ))
    {
        ST_destroy_config(c);
        ST_abort(
            __FILE__,
            __LINE__,
            "stored.server value error"
        );
    }
}

ST_CONFIG ST_new_config(const char *conf_file_path)
{
    ST_CONFIG c;
    int pthread_stat;
    file_path = conf_file_path;

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

    pthread_stat = pthread_mutex_init(&c->mutex, NULL);
    if(pthread_stat)
    {
        config_destroy(&cfg);
        free(c);
        ST_abort(
            __FILE__,
            __LINE__,
            strerror(pthread_stat)

        );
    }
    load_from_file(c);
    return c;
}

void ST_reload_config(ST_CONFIG c)
{
    ST_lock(&c->mutex);
    load_from_file(c);
    ST_unlock(&c->mutex);
}

void ST_print_config(ST_CONFIG c)
{
    printf(
        "config{\n"
            " interval \t %d;\n"
            " notice_percent \t %d;\n"
            " warn_percent \t %d;\n"
            " crit_percent \t %d;\n"
            " http_enabled \t %d;\n"
            " http_port \t %d;\n"
            " http_bind_address %s;\n"
        "}\n",
    c->interval,
    c->notice_percent,
    c->warn_percent,
    c->crit_percent,
    c->http_enabled,
    c->http_port,
    c->http_bind_address
    );
}



void ST_destroy_config(ST_CONFIG c)
{
    config_destroy(&cfg);
    pthread_mutex_destroy(&c->mutex);
    free(c);
}