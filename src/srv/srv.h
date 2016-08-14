#ifndef SRV_H
#define SRV_H
#include <pthread.h>

pthread_mutex_t mxq; /* mutex used as quit flag */




void init_server(void);

void* run_server(void *);

void stop_server(void);

#endif