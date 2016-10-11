/* vim: set tabstop=2 expandtab: */
#include <stdio.h>
#include <stdlib.h>
#include <mntent.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/statvfs.h>
#include "mtab_checker.h"
#include "util/common.h"
#include "util/logger.h"

//#define TYPE_EXC_LEN  3

typedef struct mntent M_TAB;
static FILE* mtabf;
static struct statvfs s;
static ST_CONFIG config;
//static char type_exc[TYPE_EXC_LEN];

ST_MTAB_ENTRIES ST_init_mtab_checker(ST_CONFIG c)
{
    ST_MTAB_ENTRIES me;
    int pthread_stat;

    if(!c)
    {
        ST_abort(
            __FILE__,
            __LINE__,
            "ST_CONFIG (c) NPE"
        );
    }
    config = c;

    mtabf = setmntent(_PATH_MOUNTED, "r");
    if(!mtabf)
    {
        ST_abort(
            __FILE__,
            __LINE__,
            "Setmntent fail"
        );
    }


    me = (ST_MTAB_ENTRIES) malloc(sizeof(ST_MTAB_MTAB_ENTRIES_STRUCT));
    if(!me)
    {
        ST_abort(
            __FILE__,
            __LINE__,
            "OOM error. Exiting!"
        );
    }

    pthread_stat = pthread_mutex_init(&me->mutex, NULL);
    if(pthread_stat)
    {
        free(me);
        ST_abort(
            __FILE__,
            __LINE__,
            strerror(pthread_stat)
        );
    }
    return me;
}

void ST_check_mtab(ST_MTAB_ENTRIES me)
{
    M_TAB* mt;
    int free_percent, notice_level;
    rewind(mtabf);

    ST_lock(&config->mutex);
    notice_level = config->notice_percent;
    ST_unlock(&config->mutex);

    ST_lock(&me->mutex);
//    map_free_strings(me->entries);
    while((mt = getmntent(mtabf)))
    {
        if(0 == statvfs(mt->mnt_dir, &s))
        {
            if(0 < s.f_blocks)
            {
                free_percent = (int)
                (
                    100.0 /
                    (
                        (
                            s.f_blocks - (s.f_bfree - s.f_bavail)
                        ) *
                        s.f_bsize
                    ) *
                    (
                        s.f_bavail * s.f_bsize
                    )
                );
                if(notice_level  >= free_percent)
                {
                //http://troydhanson.github.io/uthash/userguide.html
                    ST_MTAB_ENTRY s;
                    s = (ST_MTAB_ENTRY)malloc(sizeof(ST_MTAB_ENTR));
                    s->key_path = strdup(mt->mnt_dir);
                    s->free_percent_val = free_percent;
                    HASH_ADD_STR(&me->entries, key_path, s );
                }
            }
        }
        else
        {
            ST_destroy_mtab_checker(me);
            ST_abort(
                __FILE__,
                __LINE__,
                "statvfs error"
            );
        }
    }
    ST_unlock(&me->mutex);
}

void ST_destroy_mtab_checker(ST_MTAB_ENTRIES me)
{
    if( 0 == endmntent(mtabf))
    {
        ST_abort(
            __FILE__,
            __LINE__,
            "endmntent fail"
        );
        ST_logger_msg("endmntent fail", ST_MSG_ERROR);
    }
    // iterate over uthash and free
    pthread_mutex_destroy(&me->mutex);
    free(me);
}
