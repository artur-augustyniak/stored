#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <mntent.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <sys/statvfs.h>
#include "mtab_check.h"
#include "util/logger.h"

#define CAPACITY  200

typedef struct mntent M_TAB;

static FILE* mtabf;
static struct statvfs s;
static ST_conf config;

static void append_notice(int pos, char *path, int percent){
    if(NULL == entries[pos])
    {
        entries[pos] = malloc(sizeof(ST_NOTICED_ENTRY));
        strcpy (entries[pos]->path,path);
        entries[pos]->free_percent = percent;
    }
    else
    {
        strcpy(entries[pos]->path, path);
        entries[pos]->free_percent = percent;
    }
    entries_count++;
}

static void destory_current_notices(void){
    for(int i = 0 ; i < CAPACITY; i++)
    {
        if(NULL != entries[i])
        {
            free(entries[i]);
            entries[i] = NULL;
        }
    }
    free(entries);
}

void ST_init_check_mtab(ST_conf conf)
{
    config = conf;
    mtabf = setmntent(_PATH_MOUNTED, "r");
    if(!mtabf)
    {
        ST_logger_msg("setmntent fail", ST_MSG_ERROR);
    }
    entries = calloc(CAPACITY, sizeof(ST_NE));
}

void ST_check_mtab(void)
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
}

void ST_destroy_check_mtab(void)
{

    if( 0 == endmntent(mtabf))
    {
        ST_logger_msg("endmntent fail", ST_MSG_ERROR);
    }
    destory_current_notices();
}