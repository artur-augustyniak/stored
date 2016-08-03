#include <stdio.h>
#include <stdlib.h>
#include <fstab.h>
#include <mntent.h>


/**
gcc -g -Wall -pedantic -std=c99  libc_fstab.c && valgrind --leak-check=yes --track-origins=yes -v  ./a.out
*/

typedef struct fstab FS_TAB;

int main()
{
    printf("%s, %s \n", _PATH_FSTAB, _PATH_MNTTAB);
    if(0 == setfsent()){
        perror("setfsent fail");
    }
    FS_TAB* ft;
    while((ft = getfsent())){
        printf("########## MOUNT: \n");
        printf("device:      \t%s\n", ft->fs_spec);
        printf("mount point: \t%s\n", ft->fs_file);
    }

    endfsent();
    return EXIT_SUCCESS;
}

