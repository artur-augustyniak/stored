#include <stdio.h>
#include <stdlib.h>
#include <mntent.h>
#include <string.h>
#ifndef IS_DAEMON
    #include <time.h>
#endif
#include <linux/limits.h>
#include "mtab_check.h"
#include "util/logger.h"

typedef struct mntent M_TAB;

#define MSG_FMT "device: %s - mount point: %s"
#define MSG_FMT_LEN 29
#define MSG_LEN MSG_FMT_LEN + PATH_MAX

const static char msg_fmt[] = MSG_FMT;
static FILE* mtabf;
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
    while((mt = getmntent(mtabf))){
        sprintf(buf, msg_fmt, mt->mnt_fsname,  mt->mnt_dir);
        put_notice(buf);
        #ifndef IS_DAEMON
            printf("timer event: %u\n", (unsigned int)time(0));
        #endif
    }
    put_notice("mount check triggered");
}