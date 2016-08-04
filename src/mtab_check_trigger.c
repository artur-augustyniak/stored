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
#include <syslog.h>

#define AUTO_CHECK_INTERVAL 60000

void trigger_check(void (*f)(void))
{
        struct sockaddr_nl nls;
        struct pollfd pfd;
        char buf[512];

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
            perror("Bind failed");
            syslog (LOG_ERR, "%s", "Bind failed");
        }

        while (-1!=poll(&pfd, 1, AUTO_CHECK_INTERVAL)) {
                f();
                int i, len = recv(pfd.fd, buf, sizeof(buf), MSG_DONTWAIT);
                if (len == -1) {
                    i = 0;
                }
                while (i<len) {
                    syslog (LOG_NOTICE, "%s", buf+i);
                    i += strlen(buf+i)+1;
                }
        }
}