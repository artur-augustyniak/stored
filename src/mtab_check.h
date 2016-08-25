#ifndef MTAB_CHECK_H
#define MTAB_CHECK_H
#include <stdio.h>
#include<pthread.h>
#include <linux/limits.h>

typedef struct ST_NOTICED_ENTRY
{
    char path[PATH_MAX];
    int free_percent;
}
ST_NOTICED_ENTRY, *ST_NE;

extern int ST_notice_level;
extern int ST_warn_level;
extern int ST_crit_level;

pthread_mutex_t ST_entries_lock;

void ST_check_mtab(void);

void ST_report_list(FILE *stream);

#endif