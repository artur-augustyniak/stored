#ifndef DEMONIZER_H
#define DEMONIZER_H
#include <signal.h>

typedef enum _ST_OP_MODE
{
    ST_FORKING,
    ST_NOTIFY
}
ST_OP_MODE;

typedef void (*ST_signal_hook)(void);

void ST_init_demonizer(ST_OP_MODE mode);

int ST_add_signal_hook(int sig, void (*signal_hook)(void));

void ST_demonize(void);

void ST_destroy_demonizer(void);

#endif