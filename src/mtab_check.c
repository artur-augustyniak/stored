#include "mtab_check.h"
#include <stdio.h>
#include <stdlib.h>
#include <mntent.h>
#include <syslog.h>

typedef struct mntent M_TAB;

void check(void)
{
    FILE* mtabf = setmntent(_PATH_MOUNTED, "r");
    if(!mtabf){
        perror("setmntent fail");
    }
    M_TAB* mt;
    while((mt = getmntent(mtabf))){
        syslog (LOG_NOTICE, "device: %s - mount point: %s", mt->mnt_fsname,  mt->mnt_dir);
    }
    syslog (LOG_NOTICE, "mount check triggered");
    if( 0 == endmntent(mtabf)){
        perror("endmntent fail");
    }
}