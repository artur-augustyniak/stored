#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/inotify.h>
#include<poll.h>
#include<stdbool.h>


typedef struct inotify_event inotify_event;


int main(void)
{
    inotify_event *event;
    int fd = inotify_init();
    int wd = inotify_add_watch(fd, "/etc", IN_ACCESS);
    fprintf(stderr, "Starting watch of /etc/mtab\n");

    struct pollfd fdp;
    fdp.fd = fd;
    fdp.events = POLLIN;

    int ret;
    int length;
    char buffer[16384];
    int buffer_i;

    bool changed = false;

    while (1) {
        ret = poll(&fdp, 1, 500);
            // read
            length = read(fd, buffer, sizeof(buffer));
            // array index
            buffer_i = 0;
            event = (inotify_event*) &buffer;
            // fprintf(stderr, "%d\n", length);
            while (length - buffer_i > 0) {
                event = (inotify_event*) &buffer[buffer_i];
                fprintf(stderr, "event %i\n", event->mask);
                fprintf(stderr, "event %s\n", event->name);
//                switch (event->mask) {
//                    case IN_CLOSE_WRITE:
//                        if (event->len > 0 && strcmp(event->name, "mtab") == 0) {
//                            // wake the main thread
//                            changed = true;
//                            fprintf(stderr, "MTAB\n");
//                        }
//                        break;
//                    default:
//                        break;
//                }
                buffer_i += (sizeof(struct inotify_event) + sizeof(char) * event->len);
            }

    }
    exit(EXIT_SUCCESS);
}

