/* vim: set tabstop=2 expandtab: */
#include <stdlib.h>
#include <error.h>
#include "common.h"

void ST_abort(const char *file, unsigned int line, const char *msg) {
    error(ERR_STATUS, ERR_NUM, "file: %s:%d - %s", file, line, msg);
    abort();
}

int ST_lock(pthread_mutex_t *l) {
    return pthread_mutex_lock(l);
}

int ST_unlock(pthread_mutex_t *l) {
    return pthread_mutex_unlock(l);
}