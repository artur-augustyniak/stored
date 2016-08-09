#include "mtab_check.h"
#include <stdio.h>
#include <stdlib.h>
#include <mntent.h>
#include <string.h>
#include "util/logger.h"

typedef struct mntent M_TAB;

const char msg_fmt[] = "device: %s - mount point: %s";
size_t msg_fmt_len = 29;

void check(void)
{
    size_t one;
    size_t two;
    FILE* mtabf = setmntent(_PATH_MOUNTED, "r");
    if(!mtabf){
        put_error("setmntent fail");
    }
    M_TAB* mt;
    while((mt = getmntent(mtabf))){
        one = strlen(mt->mnt_fsname);
        two = strlen(mt->mnt_dir);
        char *buf;
        size_t sz = msg_fmt_len + one + two;
        buf = (char *)malloc(sz + 1); /* make sure you check for != NULL in real code */
        snprintf(buf, sz+1, msg_fmt, mt->mnt_fsname,  mt->mnt_dir);
        put_notice(buf);
        free(buf);
    }
    put_notice("mount check triggered");
    if( 0 == endmntent(mtabf)){
        put_error("endmntent fail");
    }
}