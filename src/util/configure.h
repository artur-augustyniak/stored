/* vim: set tabstop=2 expandtab: */
#ifndef CONFIG_H
#define CONFIG_H

#define IP_V4_LEN 16

typedef struct _ST_CONF
{
    int timeout;
    int notice_level;
    int warn_level;
    int crit_level;
    int server_enabled;
    int server_port;
    char bind_address[IP_V4_LEN];
}
ST_CONF;

ST_CONF curr_config;

void ST_init_config();

int ST_read_conf();

void ST_destroy_config();

#endif