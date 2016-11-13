/* vim: set tabstop=2 expandtab: */
#include<stdio.h>
#include <stdlib.h>
#include<pthread.h>
#include "../../src/util/logger.h"

#define THREAD_ITER 100
#define NUM_THREADS 20

void *inc_interval(void *p) {
    int i;
    for (i = 0; i < THREAD_ITER; i++) {
        printf("################ ENTRY ################\n");
        ST_logger_msg("logger test", ST_MSG_WARN);
    }
    return NULL;
}

/*  gcc -g -lconfig -lpthread logger_test.c logger.c sds.c common.c */
int main(void) {

    ST_logger_init("test", ST_STDOUT);
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
    ST_logger_destroy();

    /* Last thing that main(); should do */
    pthread_exit(NULL);
    return 0;
}
