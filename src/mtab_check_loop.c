/* vim: set tabstop=2 expandtab: */
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include "util/common.h"
#include "util/logger.h"
#include "mtab_check_loop.h"

#define MSG_FMT "%s has %i percent space left."
#define MSG_FMT_LEN 30
#define MSG_LEN MSG_FMT_LEN + PATH_MAX

static bool active = false;
static struct sockaddr_nl nls;
static struct pollfd pfd;
static char buf[512];

const static char msg_fmt[] = MSG_FMT;
static char msg_buf[MSG_LEN];

static ST_CONFIG config = NULL;
static ST_SRV_BUFF srv_buff = NULL;
static ST_MTAB_ENTRIES mtab_entries = NULL;


void ST_init_check_loop(
        ST_CONFIG c,
        ST_SRV_BUFF b,
        ST_MTAB_ENTRIES me
) {

    if (!c || !b || !me) {
        ST_abort(
                __FILE__,
                __LINE__,
                "Mtab checker init param (c, b, me) NPE"
        );
    }

    config = c;
    srv_buff = b;
    mtab_entries = me;
    active = true;
    // Open hotplug event netlink socket
    memset(&nls, 0, sizeof(struct sockaddr_nl));
    nls.nl_family = AF_NETLINK;
    nls.nl_pid = getpid();
    nls.nl_groups = -1;
    pfd.events = POLLIN;
    pfd.fd = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);
    // Listen to netlink socket
    if (bind(pfd.fd, (void *) &nls, sizeof(struct sockaddr_nl))) {
        ST_abort(
                __FILE__,
                __LINE__,
                "Hotplug event netlink socket bind failed"
        );
    }
}

void ST_check_loop(void) {
    if (!config || !srv_buff || !mtab_entries) {
        ST_abort(
                __FILE__,
                __LINE__,
                "Mtab checker uninitialized"
        );
    }
    int timeout, notice, warn, crit, free_space;
    JsonNode *entry;
    JsonNode *fspace_elem;
    JsonNode *m_point_elem;
    while (active) {
        ST_lock(&config->mutex);
        timeout = config->interval;
        notice = config->notice_percent;
        warn = config->warn_percent;
        crit = config->crit_percent;
        ST_unlock(&config->mutex);
        switch (poll(&pfd, 1, timeout)) {
            case -1: {
                if (errno == EINTR) {
                    continue;
                } else {
                    active = false;
                    ST_abort(
                            __FILE__,
                            __LINE__,
                            "Unknown poll error."
                    );
                }
            }
        }

        ST_lock(&mtab_entries->mutex);
        json_foreach(entry, mtab_entries->json_entries) {
            fspace_elem = json_find_member(entry, "free_space");
            m_point_elem = json_find_member(entry, "mount_point");
            if
                    (
                    (JSON_NUMBER == fspace_elem->tag && fspace_elem)
                    && (JSON_STRING == m_point_elem->tag && m_point_elem)
                    ) {
                free_space = (int) fspace_elem->number_;
                if (notice >= free_space) {
                    sprintf(msg_buf, msg_fmt, m_point_elem->string_, free_space);
                    if (crit >= free_space) {
                        ST_logger_msg(msg_buf, ST_MSG_CRIT);
                    } else if (warn >= free_space) {
                        ST_logger_msg(msg_buf, ST_MSG_WARN);
                    } else {
                        ST_logger_msg(msg_buf, ST_MSG_NOTICE);
                    }
                }
            } else {
                ST_logger_msg("json_foreach: readout error", ST_MSG_WARN);
            }
        }
        ST_unlock(&mtab_entries->mutex);

        switch (pthread_mutex_trylock(&srv_buff->mutex)) {
            case 0:
                ST_check_mtab(mtab_entries);
                srv_buff->data = mtab_entries->textural;
                ST_unlock(&srv_buff->mutex);
                break;
            case EBUSY:
                ST_logger_msg("srv_buff->mutex locked", ST_MSG_NOTICE);
                break;
        }

        /* Ignore recved data */
        recv(pfd.fd, buf, sizeof(buf), MSG_DONTWAIT);
    }
    close(pfd.fd);
}

void ST_break_check_loop(void) {
    active = false;
}