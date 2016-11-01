/* vim: set tabstop=2 expandtab: */
#include<stdio.h>
#include <stdlib.h>
#include<pthread.h>
#include "common.h"
#include "configure.h"
#include "srv.h"

#define THREAD_ITER 10000
#define NUM_THREADS 20

ST_CONFIG core_config = NULL;
ST_SRV_BUFF srv_buff = NULL;
char str[20];
int counter;

void *inc_interval(void *p) {
    int i;

    for (i = 0; i < THREAD_ITER; i++) {
        ST_lock(&srv_buff->mutex);
        sprintf(str, "<b>%d</b>", counter++);
        pthread_yield();
        srv_buff->data = str;
        ST_unlock(&srv_buff->mutex);

    }
}

/* gcc -g -lconfig -lpthread srv_test.c srv.c configure.c common.c */
int main(void) {

    core_config = ST_new_config("../../etc/stored.cfg");
    srv_buff = ST_init_srv(core_config);
    ST_start_srv(srv_buff);

    pthread_t threads[NUM_THREADS];
    int rc;
    long t;
    for (t = 0; t < NUM_THREADS; t++) {
        rc = pthread_create(&threads[t], NULL, &inc_interval, (void *) t);
        if (rc) {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    /* Wait for all threads to complete */
    int i;
    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    sleep(20);
    core_config->http_port = 1532;
    ST_restart_srv(srv_buff);


    sleep(20);
    ST_stop_srv(srv_buff);
    ST_destroy_srv(srv_buff);
    ST_destroy_config(core_config);


    /* Last thing that main(); should do */
    pthread_exit(NULL);
    return 0;
}
