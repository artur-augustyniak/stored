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


void init_mtab(void)
{
    mtabf = setmntent(_PATH_MOUNTED, "r");
    if(!mtabf){
        put_error("setmntent fail");
    }
}

void destroy_mtab(void)
{
    if( 0 == endmntent(mtabf)){
        put_error("endmntent fail");
    }
}

void check_mtab(void)
{
    rewind(mtabf);
    M_TAB* mt;
    int free_percent;
    while((mt = getmntent(mtabf)))
    {
        if(0 == statvfs(mt->mnt_dir, &s))
        {
            if(0 < s.f_blocks)
            {
                free_percent = (int)(100.0 /((s.f_blocks - (s.f_bfree - s.f_bavail)) * s.f_bsize) * (s.f_bavail * s.f_bsize));
                if(FREE_PERCENT_NOTICE >= free_percent)
                {
                    #ifndef IS_DAEMON
                        printf("timer event: %u\n", (unsigned int)time(0));
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
}

