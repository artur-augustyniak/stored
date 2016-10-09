/* vim: set tabstop=2 expandtab: */
#ifndef SRV_H
#define SRV_H
#include "configure.h"

typedef struct _SRV_BUFF
{
    int size;
    const char *data;
    pthread_mutex_t mutex;
} ST_SERVER_BUFFER, *ST_SRV_BUFF;

/**
 * Initialize server own thread and
 * returns handle to content buffer
 * or NULL if config has http disabled flag
 */
ST_SRV_BUFF ST_init_srv(ST_CONFIG c);

void ST_start_srv(ST_SRV_BUFF b);

void ST_stop_srv(ST_SRV_BUFF b);

void ST_restart_srv(ST_SRV_BUFF b);

void ST_destroy_srv(ST_SRV_BUFF b);

#endif