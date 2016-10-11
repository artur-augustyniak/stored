/* vim: set tabstop=2 expandtab: */
#include<stdio.h>
#include <stdlib.h>
#include<pthread.h>
#include "configure.h"

#define THREAD_ITER 10000
#define NUM_THREADS 20

ST_CONFIG core_config = NULL;

void* inc_interval(void *p)
{
    int i;
    for(i=0; i < THREAD_ITER; i++)
    {
        ST_lock(&core_config->mutex);
        core_config->interval++;
        ST_unlock(&core_config->mutex);
    }
}

/* gcc -g -lconfig -lpthread configure_test.c configure.c */
int  main(void)
{

    core_config = ST_new_config("../../../etc/stored.cfg");

    pthread_t threads[NUM_THREADS];
    int rc;
    long t;
    for(t=0; t<NUM_THREADS; t++)
    {
        rc = pthread_create(&threads[t], NULL, &inc_interval, (void *)t);
        if (rc)
        {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    /* Wait for all threads to complete */
    int i;
    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    ST_print_config(core_config);
    sleep(40);
    ST_reload_config(core_config);
    ST_print_config(core_config);


    ST_destroy_config(core_config);

    /* Last thing that main(); should do */
    pthread_exit(NULL);
    return 0;
}
