/* vim: set tabstop=2 expandtab: */
#ifndef CONFIGURE_H
#define CONFIGURE_H

typedef struct _ST_CONF
{
    int timeout;
    int notice_level;
    int warn_level;
    int crit_level;
    int server_enabled;
    int server_port;
    const char *bind_address;
}
ST_CONF, *ST_conf;

ST_conf ST_config(const char *fpath);

void ST_destroy_config();

#endif