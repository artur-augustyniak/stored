#ifndef MTAB_CHECK_H
#define MTAB_CHECK_H
#include<pthread.h>
#include <linux/limits.h>

typedef struct ST_NOTICED_ENTRY
{
    char path[PATH_MAX];
    int free_percent;
}
ST_NOTICED_ENTRY, *ST_NE;

pthread_mutex_t ST_entries_lock;

__BEGIN_DECLS

void ST_check_mtab(void);

void ST_report_list(FILE *stream);

__END_DECLS

#endif