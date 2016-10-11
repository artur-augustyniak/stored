/* vim: set tabstop=2 expandtab: */
#include<stdio.h>
#include <stdlib.h>
#include<pthread.h>
#include "util/configure.h"
#include "mtab_checker.h"

#define THREAD_ITER 10
#define NUM_THREADS 20

ST_CONFIG core_config = NULL;
ST_MTAB_ENTRIES entries = NULL;




void* inc_interval(void *p)
{
    int i;
    for(i=0; i < THREAD_ITER; i++)
    {
        ST_check_mtab(entries);
        ST_lock(&entries->mutex);
        pthread_yield();
        ST_unlock(&entries->mutex);
    }
}

/* gcc -g -lconfig -lpthread mtab_checker_test.c mtab_checker.c util/configure.c util/map_lib.c util/logger.c util/sds.c  */
int  main(void)
{

    core_config = ST_new_config("../../etc/stored.cfg");
    entries = ST_init_mtab_checker(core_config);
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
    ST_check_mtab(entries);
    ST_destroy_mtab_checker(entries);
    ST_destroy_config(core_config);

    /* Last thing that main(); should do */
    pthread_exit(NULL);
    return 0;
}
