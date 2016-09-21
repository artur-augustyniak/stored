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

ST_NE *entries;

int entries_count;

void ST_init_check_mtab(ST_conf conf);

void ST_check_mtab(void);

void ST_destroy_check_mtab(void);

#endif