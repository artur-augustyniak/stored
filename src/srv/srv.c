#include <config.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include "srv.h"
#include "mtab_check.h"


static int quit(pthread_mutex_t *mtx)
{
  switch(pthread_mutex_trylock(mtx)) {
    case 0: /* if we got the lock, unlock and return 1 (true) */
      pthread_mutex_unlock(mtx);
      return 1;
    case EBUSY: /* return 0 (false) if the mutex was locked */
      return 0;
  }
  return 1;
}

void init_server(void){
    pthread_mutex_init(&mxq,NULL);
    pthread_mutex_lock(&mxq);
}


void* run_server(void *arg)
{
    pthread_mutex_t *mx = arg;
    while(!quit(mx))
    {
        sleep(5);
        printf("from server\n");
        report_list();
    }
    return NULL;
}


void stop_server(void){
     pthread_mutex_unlock(&mxq);
}