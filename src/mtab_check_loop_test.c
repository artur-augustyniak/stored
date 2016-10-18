/* vim: set tabstop=2 expandtab: */
#include<stdio.h>
#include <stdlib.h>
#define __USE_GNU
#include <pthread.h>
#undef __USE_GNU
#include <signal.h>
#include "util/common.h"
#include "util/configure.h"
#include "util/logger.h"
#include "util/srv.h"
#include "mtab_check_loop.h"
#include "mtab_checker.h"


#define THREAD_ITER 1000
#define NUM_THREADS 20

ST_CONFIG core_config = NULL;
ST_MTAB_ENTRIES entries = NULL;
ST_SRV_BUFF srv_buff = NULL;

static void reload(int sig)
{
    ST_logger_msg("daemon reloading.", ST_MSG_NOTICE);
    ST_reload_config(core_config);
    ST_restart_srv(srv_buff);
}

static void stop(int sig)
{
    ST_logger_msg("daemon terminating.", ST_MSG_NOTICE);
    ST_break_check_loop();
}

void* inc_interval(void *p)
{
    int i;
    for(i=0; i < THREAD_ITER; i++)
    {
        printf("################ ENTRY ################\n");
        ST_check_mtab(entries);
        ST_lock(&entries->mutex);
        pthread_yield();
        printf("%s\n", entries->textural);
        ST_unlock(&entries->mutex);
    }
    return NULL;
}

/*
gcc -std=gnu11 -Wall -pedantic -g -lconfig -pthread mtab_check_loop_test.c mtab_check_loop.c mtab_checker.c util/configure.c util/logger.c util/sds.c util/common.c util/json.c util/srv.c
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -v ./a.out
kill -SIGINT `ps aux | grep "a\.out" | awk '{print $2}'`
 */
int  main(void)
{

    signal(SIGINT, &stop);
    signal(SIGHUP, &reload);

    ST_logger_init("test", ST_STDOUT);
    core_config = ST_new_config("../etc/stored.cfg");
    entries = ST_init_mtab_checker(core_config);
    srv_buff = ST_init_srv(core_config);
    srv_buff->data = "{}";
    ST_start_srv(srv_buff);
    ST_init_check_loop(core_config, srv_buff, entries);

    ST_check_loop();

//    pthread_t threads[NUM_THREADS];
//    int rc;
//    long t;
//    for(t=0; t<NUM_THREADS; t++)
//    {
//        rc = pthread_create(&threads[t], NULL, &inc_interval, (void *)t);
//        if (rc)
//        {
//            printf("ERROR; return code from pthread_create() is %d\n", rc);
//            exit(-1);
//        }
//    }
//
//    /* Wait for all threads to complete */
//    int i;
//    for (i = 0; i < NUM_THREADS; i++) {
//        pthread_join(threads[i], NULL);
//    }

    ST_stop_srv(srv_buff);
    ST_destroy_srv(srv_buff);
    ST_destroy_mtab_checker(entries);

    ST_destroy_config(core_config);
    ST_logger_destroy();

    /* Last thing that main(); should do */
    pthread_exit(NULL);
    return 0;
}
