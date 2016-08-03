#include <stdio.h>
#include <stdlib.h>
#include <fstab.h>
#include <mntent.h>


/**
gcc -g -Wall -pedantic -std=c99  libc_mtab.c && valgrind --leak-check=yes --track-origins=yes -v  ./a.out
gcc -g -Wall -pedantic -std=c99  libc_mtab.c &&  ./a.out
*/

typedef struct mntent M_TAB;

int main()
{
    printf("%s, %s \n", _PATH_FSTAB, _PATH_MOUNTED);


    FILE* mtabf = setmntent(_PATH_MOUNTED, "r");
    if(!mtabf){
        perror("setmntent fail");
        return 127;
    }

    M_TAB* mt;

    for(;;){
        rewind(mtabf);
        system("clear");
        while((mt = getmntent(mtabf))){
            printf("########## MOUNT MTAB: \n");
            printf("device:      \t%s\n", mt->mnt_fsname);
            printf("mount point: \t%s\n", mt->mnt_dir);
        }
        usleep(10000);
    }
    if( 0 == endmntent(mtabf)){
        perror("endmntent fail");
    }
    return EXIT_SUCCESS;
}

