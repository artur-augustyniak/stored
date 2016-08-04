#include "mtab_check_trigger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <linux/types.h>
#include <linux/netlink.h>

void trigger_check(void (*f)(void))
{
    struct sockaddr_nl nls;
    struct pollfd pfd;
    memset(&nls,0,sizeof(struct sockaddr_nl));
    nls.nl_family = AF_NETLINK;
    nls.nl_pid = getpid();
    nls.nl_groups = -1;

    pfd.events = POLLIN;
    pfd.fd = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);
    while (-1!=poll(&pfd, 1, 100))
    {
        f();
    }
}