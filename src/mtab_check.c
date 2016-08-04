#include "mtab_check.h"
#include <stdio.h>
#include <stdlib.h>
#include <mntent.h>

typedef struct mntent M_TAB;

void check(void)
{
    FILE* mtabf = setmntent(_PATH_MOUNTED, "r");
    if(!mtabf){
        perror("setmntent fail");
    }
    M_TAB* mt;
    while((mt = getmntent(mtabf))){
        printf("########## MOUNT MTAB: \n");
        printf("device:      \t%s\n", mt->mnt_fsname);
        printf("mount point: \t%s\n", mt->mnt_dir);
    }
    if( 0 == endmntent(mtabf)){
        perror("endmntent fail");
    }
}