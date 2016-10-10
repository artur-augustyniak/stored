#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <mntent.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/statvfs.h>
#include "mtab_check.h"
#include "util/common.h"
#include "util/logger.h"

typedef struct mntent M_TAB;
static FILE* mtabf;
static struct statvfs s;

ST_MTAB_ENTRIES ST_init_mtab_checker(ST_CONFIG c)
{

    ST_MTAB_ENTRIES me;
    int pthread_stat;

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
    me->entries = map_create();
    if(!me->entries)
    {
        free(me);
        ST_abort(
            __FILE__,
            __LINE__,
            "OOM error. Exiting!"
        );
    }
    return me;
}

void ST_check_mtab(ST_MTAB_ENTRIES me)
{
    if(!active)
    {
        ST_logger_msg("mtab_check not initialized.", ST_MSG_CRIT);
        exit(EXIT_FAILURE);
    }
    pthread_mutex_lock(&ST_entries_lock);
    rewind(mtabf);
    M_TAB* mt;
    int free_percent;
    entries_count = 0;
    while((mt = getmntent(mtabf)))
    {
        if(0 == statvfs(mt->mnt_dir, &s))
        {
            if(0 < s.f_blocks)
            {
                free_percent = (int)(100.0 /((s.f_blocks - (s.f_bfree - s.f_bavail)) * s.f_bsize) * (s.f_bavail * s.f_bsize));
                if(config->notice_level  >= free_percent)
                {
                    append_notice(entries_count, mt->mnt_dir, free_percent);
                }
            }
        }
        else
        {
            ST_logger_msg("statvfs error", ST_MSG_ERROR);
        }
    }

    //Halving down
    if(entries_count > 0 && entries_count == (int) (runtime_entries_capacity-1)/ 4)
    {
        ST_NE *tmp_entries;
        int old_capacity = runtime_entries_capacity;
        runtime_entries_capacity /=2;
        for(int i = runtime_entries_capacity; i < old_capacity; i++)
        {
            if(NULL != entries[i])
            {
                free(entries[i]);
                entries[i] = NULL;
            }
        }
        tmp_entries = (ST_NE* )realloc(entries, runtime_entries_capacity * sizeof(ST_NE));
        entries = tmp_entries;
    }
    pthread_mutex_unlock(&ST_entries_lock);
}


void ST_destroy_mtab_checker(ST_MTAB_ENTRIES me)
{
    if( 0 == endmntent(mtabf))
    {
        ST_logger_msg("endmntent fail", ST_MSG_ERROR);
    }
    map_free_strings(me->entries);
    pthread_mutex_destroy(&me->mutex);
    free(me);
}



//#######################OLD
//static int entries_count;
//static int runtime_entries_capacity;
//static bool active = false;
//
//static void append_notice(int pos, char *path, int percent){
//    //Halving up
//    if(entries_count == runtime_entries_capacity-1){
//        ST_NE *tmp_entries;
//
//        runtime_entries_capacity *=2;
//        tmp_entries = (ST_NE* )realloc(entries, runtime_entries_capacity * sizeof(ST_NE));
//        entries = tmp_entries;
//        for(int i = entries_count; i < runtime_entries_capacity; i++)
//        {
//                entries[i] = NULL;
//        }
//    }
//
//    if(NULL == entries[pos])
//    {
//        entries[pos] = malloc(sizeof(ST_NOTICED_ENTRY));
//        strcpy (entries[pos]->path,path);
//        entries[pos]->free_percent = percent;
//    }
//    else
//    {
//        strcpy(entries[pos]->path, path);
//        entries[pos]->free_percent = percent;
//    }
//    entries_count++;
//}
//
//static void destory_current_notices(void){
//    pthread_mutex_lock(&ST_entries_lock);
//    for(int i = 0 ; i < runtime_entries_capacity; i++)
//    {
//        if(NULL != entries[i])
//        {
//            free(entries[i]);
//            entries[i] = NULL;
//        }
//    }
//    free(entries);
//    pthread_mutex_unlock(&ST_entries_lock);
//}
//
//void ST_init_check_mtab(ST_conf conf)
//{
//    config = conf;
//    active = true;
//    mtabf = setmntent(_PATH_MOUNTED, "r");
//    if(!mtabf)
//    {
//        ST_logger_msg("setmntent fail", ST_MSG_ERROR);
//    }
//    runtime_entries_capacity = DEFAULT_NOTIFICATION_CAPACITY;
//    entries = calloc(runtime_entries_capacity, sizeof(ST_NE));
//    pthread_mutex_init(&ST_entries_lock, NULL);
//}
