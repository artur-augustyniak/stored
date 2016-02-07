#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <linux/fanotify.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <fcntl.h>

/**
 * gcc -o notifier notifier.c
 * ./notifier /home/file /dev/shm/monit 10
 * Arguments are as follows:
 * A file on the filesystem you want to monitor.
 * A path to a file that will be created if you go over threshold (and be deleted if under)
 * A percentage of free space that should be available to be under threshold.
*/
int main(const int argc, const char **argv) {
    if (argc < 4) {
        fprintf(stderr, "Supply a path to a file on the mountpoint to listen to, a monitor file and a free %% threshold..\n");
        exit(1);
    }

    if (access(argv[1], R_OK) < 0) {
        fprintf(stderr, "Unable to read file: %s\n", strerror(errno));
        exit(1);
    }

    int len, rc;
    unsigned char donestat = 0,  alerted = 0;
        const char *path = argv[1];
    const char *monpath = argv[2];
    int threshold = atoi(argv[3]);
    char buf[4096];
    struct fanotify_event_metadata *fem = NULL;
    int fan_fd = -1;
    uint64_t mask = FAN_CLOSE_WRITE;
    struct statvfs sfs;
    float bfree;

    memset(&sfs, 0, sizeof(sfs));
    unlink(monpath);

    if (threshold <= 0 || threshold >= 100) {
        fprintf(stderr, "Incorrect threshold provided");
        rc = 1;
        goto end;
    }

    fan_fd = fanotify_init(FAN_CLASS_NOTIF, FAN_CLOEXEC);
    if (fan_fd < 0) {
        perror("fanotify_init");
        rc = 1;
        goto end;
    }

    rc = fanotify_mark(fan_fd, FAN_MARK_ADD|FAN_MARK_MOUNT, mask, AT_FDCWD, path);
    if (rc < 0) {
        perror("fanotify_mark");
        rc = 1;
        goto end;
    }

    while ((len = read(fan_fd, buf, sizeof(buf))) > 0) {
        fem = (void *)buf;
        donestat = 0;

        while (FAN_EVENT_OK(fem, len)) {
            if (fem->vers < 2) {
                fprintf(stderr, "fanotify is too old\n");
                goto end;
            }

            if (!donestat) {
                rc = fstatvfs(fem->fd, &sfs);
                if (rc < 0) {
                    perror("fstatvfs");
                    rc = 1;
                    goto end;
                }
                bfree = 100 - (((float)(sfs.f_blocks - ((sfs.f_blocks - sfs.f_bfree))) / (float)(sfs.f_blocks)) * 100);
                if ((bfree < (float)threshold)) {
                    if (!alerted) {
                        creat(monpath, S_IRUSR|S_IWUSR);
                        alerted = 1;
                    }
                }
                else {
                    if (alerted) {
                        unlink(monpath);
                        alerted = 0;
                    }
                }
            }
            donestat = 1;
            close(fem->fd);
            fem = FAN_EVENT_NEXT(fem, len);
        }
    }
    if (len < 0) {
        perror("Read fan_fd");
        rc = 1;
        goto end;
    }

end:
    close(fan_fd);
    exit(rc);
}
