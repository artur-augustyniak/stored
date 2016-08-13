#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <linux/types.h>
#include <linux/netlink.h>
#include "mtab_check.h"
#include "mtab_check_trigger.h"
#include "util/logger.h"

static bool active = true;
static struct sockaddr_nl nls;
static struct pollfd pfd;
static char buf[512];

void init_checks_loop(void)
{
    // Open hotplug event netlink socket
    memset(&nls,0,sizeof(struct sockaddr_nl));
    nls.nl_family = AF_NETLINK;
    nls.nl_pid = getpid();
    nls.nl_groups = -1;

    pfd.events = POLLIN;
    pfd.fd = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);

    // Listen to netlink socket
    if (bind(pfd.fd, (void *)&nls, sizeof(struct sockaddr_nl)))
    {
            put_error("Bind failed");
    }
}

void break_checks_loop(void)
{
    active = false;
}

void report_list()
{
    NE c = entries_handle;
    while (c != NULL)
    {
        printf("ENTRY: %s %i %p\n", c->path, c->free_percent, c->next);
        c = c->next;
    }
}


void checks_loop(void (*check_func)(void))
{
        while (-1!=poll(&pfd, 1, AUTO_CHECK_INTERVAL) && active) {
                check_func();
                printf("#############################################\n");
                report_list();
                printf("#############################################\n");
                recv(pfd.fd, buf, sizeof(buf), MSG_DONTWAIT);
        }
}