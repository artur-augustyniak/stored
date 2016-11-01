/* vim: set tabstop=2 expandtab: */
#ifndef ST_COMMON_H
#define ST_COMMON_H

#include<pthread.h>

#define ERR_STATUS  1
#define ERR_NUM     0

void ST_abort(const char *file, unsigned int line, const char *msg);

int ST_lock(pthread_mutex_t *l);

int ST_unlock(pthread_mutex_t *l);

#endif