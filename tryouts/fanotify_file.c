#define _GNU_SOURCE     /* Needed to get O_LARGEFILE definition */
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <poll.h>
#include <sys/fanotify.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstab.h>
#include <mntent.h>
#include <unistd.h>
#include <sys/inotify.h>
/**
gcc -g -Wall -pedantic -std=c99  fanotify_file.c && valgrind --leak-check=yes --track-origins=yes -v  ./a.out
gcc -g -Wall -pedantic -std=c99  fanotify_file.c &&  ./a.out
*/
#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define EVENT_BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )
typedef struct mntent M_TAB;
typedef struct inotify_event EVENT;

int main()
{
    printf("%s, %s \n", _PATH_FSTAB, _PATH_MOUNTED);
   
    int fd = fanotify_init(FAN_CLASS_NOTIF, O_RDONLY);

//    int fd = fanotify_init(FAN_CLOEXEC | FAN_CLASS_CONTENT | FAN_NONBLOCK,
//                              O_RDONLY | O_LARGEFILE);

    
    int wd = fanotify_mark(
        fd,
        FAN_MARK_ADD,
        FAN_MODIFY,
        0,
        "/proc/mounts"
    );
    const struct fanotify_event_metadata *metadata;
    struct fanotify_event_metadata buf[200];
    ssize_t len;
    char path[PATH_MAX];
    ssize_t path_len;
    char procfd_path[PATH_MAX];
    struct fanotify_response response;

           /* Loop while events can be read from fanotify file descriptor */

           for(;;) {

               /* Read some events */

               len = read(fd, (void *) &buf, sizeof(buf));
               if (len == -1 && errno != EAGAIN) {
                   perror("read");
                   exit(EXIT_FAILURE);
               }

               /* Check if end of available data reached */

               if (len <= 0)
                   break;

               /* Point to the first event in the buffer */

               metadata = buf;

               /* Loop over all events in the buffer */

               while (FAN_EVENT_OK(metadata, len)) {

                   /* Check that run-time and compile-time structures match */

                   if (metadata->vers != FANOTIFY_METADATA_VERSION) {
                       fprintf(stderr,
                               "Mismatch of fanotify metadata version.\n");
                       exit(EXIT_FAILURE);
                   }

                   /* metadata->fd contains either FAN_NOFD, indicating a
                      queue overflow, or a file descriptor (a nonnegative
                      integer). Here, we simply ignore queue overflow. */

                   if (metadata->fd >= 0) {

                       /* Handle open permission event */

                       if (metadata->mask & FAN_OPEN_PERM) {
                           printf("FAN_OPEN_PERM: ");

                           /* Allow file to be opened */

                           response.fd = metadata->fd;
                           response.response = FAN_ALLOW;
                           write(fd, &response,
                                 sizeof(struct fanotify_response));
                       }

                       /* Handle closing of writable file event */

                       if (metadata->mask & FAN_CLOSE_WRITE)
                           printf("FAN_CLOSE_WRITE: ");

                       /* Retrieve and print pathname of the accessed file */

                       snprintf(procfd_path, sizeof(procfd_path),
                                "/proc/self/fd/%d", metadata->fd);
                       path_len = readlink(procfd_path, path,
                                           sizeof(path) - 1);
                       if (path_len == -1) {
                           perror("readlink");
                           exit(EXIT_FAILURE);
                       }

                       path[path_len] = '\0';
                       printf("File %s\n", path);

                       /* Close the file descriptor of the event */

                       close(metadata->fd);
                   }

                   /* Advance to next event */

                   metadata = FAN_EVENT_NEXT(metadata, len);
               }
           }



//    int length;
//    char buffer[EVENT_BUF_LEN];
//    EVENT* ev;
//
//    for(;;)
//    {
//        length = read(fd, buffer, EVENT_BUF_LEN);
//        if (length < 0)
//        {
//            perror( "inotify read error");
//        }
//        i = 0;
//        while (i < length)
//        {
//            ev = (EVENT*) &buffer[i];
//            if(ev->mask & IN_MODIFY)
//            {
//                printf( "mtab modified. %s\n", ev->name);
//            }
//            i += EVENT_SIZE + ev->len;
//        }
//    }
//    inotify_rm_watch(fd, wd);
//    close(fd);
    return EXIT_SUCCESS;
}

