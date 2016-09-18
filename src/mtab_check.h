#ifndef MTAB_CHECK_H
#define MTAB_CHECK_H
#include <linux/limits.h>
#include "util/configure.h"

typedef struct ST_NOTICED_ENTRY
{
    char path[PATH_MAX];
    int free_percent;
}
ST_NOTICED_ENTRY, *ST_NE;

typedef  ST_NE *ENTRIES;

pthread_mutex_t ST_entries_lock;

void ST_init_check_mtab(ST_conf conf);

void ST_check_mtab(void);

ENTRIES ST_get_entries(int *count);

void ST_destroy_check_mtab(void);

#endif