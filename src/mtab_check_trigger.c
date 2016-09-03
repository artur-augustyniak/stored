#include <config.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include "util/logger.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>


static bool active = false;
static struct sockaddr_nl nls;
static struct pollfd pfd;

static char buf[512];

int ST_timeout = AUTO_CHECK_INTERVAL;

static void init_checks_loop(void)
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
            ST_msg("Bind failed", ST_MSG_ERROR);
    }
}

void ST_break_checks_loop(void)
{
    active = false;
}

void ST_checks_loop(void (*check_func)(void))
{
        if(!active){
            active = true;
            init_checks_loop();
        }

        while(active)
        {
            switch (poll(&pfd, 1, ST_timeout))
            {
                case -1:{
                    if (errno == EINTR)
                    {
                        continue;
                    }
                    else
                    {
                        active = false;
                        perror("unknown poll error");
                        exit(1);
                    }
                }
            }
            check_func();
            /* Ignore recved data */
            recv(pfd.fd, buf, sizeof(buf), MSG_DONTWAIT);
        }
}