#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include "util/logger.h"
#include "mtab_check.h"
#include "mtab_check_trigger.h"

#define MSG_FMT "%s has %i percent space left."
#define MSG_FMT_LEN 30
#define MSG_LEN MSG_FMT_LEN + PATH_MAX

static ST_conf config;
static bool active = false;
static struct sockaddr_nl nls;
static struct pollfd pfd;
static char buf[512];

const static char msg_fmt[] = MSG_FMT;
static char msg_buf[MSG_LEN];


void ST_init_checks_loop(ST_conf conf)
{
    config = conf;
    active = true;
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
        ST_logger_msg("Bind failed", ST_MSG_ERROR);
    }
}

void ST_checks_loop(void (*check_func)(void), int timeout)
{
    ENTRIES curr_list = NULL;
    ST_NE entry =  NULL;
    int items;
    while(active)
    {
        switch (poll(&pfd, 1, timeout))
        {
            case -1:
            {
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
        curr_list = ST_get_entries(&items);
        for(int i = 0;i < items; i++)
        {
            entry = curr_list[i];
            if(config->notice_level  >= entry->free_percent)
            {
                sprintf(msg_buf, msg_fmt, entry->path, entry->free_percent);
                if(config->crit_level  >= entry->free_percent)
                {
                    ST_logger_msg(msg_buf, ST_MSG_CRIT);
                }
                else if(config->warn_level  >= entry->free_percent)
                {
                    ST_logger_msg(msg_buf, ST_MSG_WARN);
                }
                else
                {
                    ST_logger_msg(msg_buf, ST_MSG_NOTICE);
                }
            }
        }
        /* Ignore recved data */
        recv(pfd.fd, buf, sizeof(buf), MSG_DONTWAIT);
    }
    close(pfd.fd);
}

void ST_break_checks_loop(void)
{
    active = false;
}