/* vim: set tabstop=2 expandtab: */
#include<stdio.h>
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
ST_CONF, *ST_CONFP;

ST_CONF a;


void main(void)
{
    a.timeout  = 110;
    printf("%i\n", a.timeout);

}
