#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <mntent.h>
#include <string.h>
#ifndef IS_DAEMON
    #include <time.h>
#endif
#include <linux/limits.h>
#include <sys/statvfs.h>
#include "mtab_check.h"
#include "util/logger.h"

typedef struct mntent M_TAB;

#define MSG_FMT "%s has %i percent space left."
#define MSG_FMT_LEN 30
#define MSG_LEN MSG_FMT_LEN + PATH_MAX

const static char msg_fmt[] = MSG_FMT;
static FILE* mtabf;
static struct statvfs s;
static char buf[MSG_LEN];


static void append_notice(int pos, char *path, int percent){
    pthread_mutex_lock(&entries_lock);
    //Halving up
    if(entries_count == runtime_entries_capacity-1){
        NE *tmp_entries;

        runtime_entries_capacity *=2;
        tmp_entries = (NE* )realloc(entries, runtime_entries_capacity * sizeof(NE));
        entries = tmp_entries;
        for(int i = entries_count; i < runtime_entries_capacity; i++)
        {
                entries[i] = NULL;
        }
    }

    if(NULL == entries[pos])
    {
        entries[pos] = malloc(sizeof(NOTICED_ENTRY));
        entries[pos]->path = strdup(path);
        entries[pos]->free_percent = percent;
    }
    else
    {
        size_t old_str = strlen(entries[pos]->path)+1;
        size_t new_str = strlen(path)+1;
        if(new_str > old_str)
        {
            char *tmp;
            tmp = (char*)realloc(entries[pos]->path, new_str);
            entries[pos]->path = tmp;
        }
        strcpy(entries[pos]->path, path);
        entries[pos]->free_percent = percent;
    }
    pthread_mutex_unlock(&entries_lock);
}

void destory_current_notices(void){
    pthread_mutex_lock(&entries_lock);
    for(int i = 0 ; i < runtime_entries_capacity; i++)
    {
        if(NULL != entries[i])
        {
            free(entries[i]->path);
            free(entries[i]);
            entries[i] = NULL;
        }
    }
    free(entries);
    pthread_mutex_unlock(&entries_lock);
}

void init_mtab(void)
{
    mtabf = setmntent(_PATH_MOUNTED, "r");
    if(!mtabf){
        put_error("setmntent fail");
    }
    runtime_entries_capacity = DEFAULT_NOTIFICATION_CAPACITY;
    entries = calloc(runtime_entries_capacity, sizeof(NE));
    pthread_mutex_init(&entries_lock, NULL);
}

void destroy_mtab(void)
{
    if( 0 == endmntent(mtabf)){
        put_error("endmntent fail");
    }
    destory_current_notices();
    pthread_mutex_destroy(&entries_lock);
}


void report_list()
{
    pthread_mutex_lock(&entries_lock);
    printf("#############################################\n");
    for(int i = 0; i < entries_count; i++)
    {
        printf("ENTRY: %s %i\n", entries[i]->path, entries[i]->free_percent);
    }
    printf("#############################################\n");
    pthread_mutex_unlock(&entries_lock);
}


void check_mtab(void)
{
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

                if(FREE_PERCENT_NOTICE >= free_percent)
                {
                    append_notice(entries_count, mt->mnt_dir, free_percent);
                    entries_count++;

                    #ifndef IS_DAEMON
                        printf("timer event: %u %s\n", (unsigned int)time(0), mt->mnt_dir);
                    #endif
                    sprintf(buf, msg_fmt, mt->mnt_dir, free_percent);
                    if(FREE_PERCENT_CRIT >= free_percent){
                        put_crit(buf);
                    }
                     else if(FREE_PERCENT_WARN >= free_percent){
                        put_warn(buf);
                     }
                     else
                     {
                        put_notice(buf);
                     }
                }
            }
        }
        else
        {
            put_error("statvfs error");
        }
    }
    pthread_mutex_lock(&entries_lock);
    //Halving down
    if(entries_count > 0 && entries_count == (int) (runtime_entries_capacity-1)/ 4)
    {
        NE *tmp_entries;
        int old_capacity = runtime_entries_capacity;
        runtime_entries_capacity /=2;
        for(int i = runtime_entries_capacity; i < old_capacity; i++)
        {
            if(NULL != entries[i])
            {
                free(entries[i]->path);
                free(entries[i]);
                entries[i] = NULL;
            }
        }
        tmp_entries = (NE* )realloc(entries, runtime_entries_capacity * sizeof(NE));
        entries = tmp_entries;
    }
    pthread_mutex_unlock(&entries_lock);
    #ifndef IS_DAEMON
        report_list();
    #endif
}

