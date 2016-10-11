#ifndef MTAB_CHECKER_H
#define MTAB_CHECKER_H
#include <linux/limits.h>
#include "util/uthash.h"
#include "util/configure.h"

typedef struct _ST_MTAB_ENTR {
    char *key_path;
    int free_percent_val;
    UT_hash_handle hh; /* makes this structure hashable */
} ST_MTAB_ENTR, *ST_MTAB_ENTRY;


typedef struct _MTAB_ENTRIES
{
    //K:string - path : V:string percent
    ST_MTAB_ENTRY entries;
    pthread_mutex_t mutex;

} ST_MTAB_MTAB_ENTRIES_STRUCT, *ST_MTAB_ENTRIES;

ST_MTAB_ENTRIES ST_init_mtab_checker(ST_CONFIG c);

void ST_check_mtab(ST_MTAB_ENTRIES me);

void ST_destroy_mtab_checker(ST_MTAB_ENTRIES me);

#endif