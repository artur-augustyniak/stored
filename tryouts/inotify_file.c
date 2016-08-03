#include <stdio.h>
#include <stdlib.h>
#include <fstab.h>
#include <mntent.h>
#include <unistd.h>
#include <sys/inotify.h>
/**
gcc -g -Wall -pedantic -std=c99  inotify_file.c && valgrind --leak-check=yes --track-origins=yes -v  ./a.out
gcc -g -Wall -pedantic -std=c99  inotify_file.c &&  ./a.out
*/
#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define EVENT_BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )
typedef struct mntent M_TAB;
typedef struct inotify_event EVENT;

int main()
{
    printf("%s, %s \n", _PATH_FSTAB, _PATH_MOUNTED);
    int i;
    int fd = inotify_init();
    int wd = inotify_add_watch(
        fd,
        "/tmp/file",
        IN_MODIFY
    );
    int length;
    char buffer[EVENT_BUF_LEN];
    EVENT* ev;

    for(;;)
    {
        length = read(fd, buffer, EVENT_BUF_LEN);
        if (length < 0)
        {
            perror( "inotify read error");
        }
        i = 0;
        while (i < length)
        {
            ev = (EVENT*) &buffer[i];
            if(ev->mask & IN_MODIFY)
            {
                printf( "mtab modified. %s\n", ev->name);
            }
            i += EVENT_SIZE + ev->len;
        }
    }
    inotify_rm_watch(fd, wd);
    close(fd);
    return EXIT_SUCCESS;
}

