/* vim: set tabstop=2 expandtab: */
#ifndef CONFIGURE_H
#define CONFIGURE_H
#include <pthread.h>

typedef struct _ST_CONF
{
    int interval;
    int notice_percent;
    int warn_percent;
    int crit_percent;
    int http_enabled;
    int http_port;
    const char *http_bind_address;
    pthread_mutex_t mutex;
} ST_CONFIG_STRUCT, *ST_CONFIG;

ST_CONFIG ST_new_config(const char *conf_file_path);

void ST_reload_config(ST_CONFIG c);

void ST_print_config(ST_CONFIG c);

void ST_destroy_config(ST_CONFIG c);

#endif