#ifndef DEMONIZER_H
#define DEMONIZER_H

typedef enum _ST_OP_MODE
{
    ST_FORKING,
    ST_NOTIFY
}
ST_OP_MODE;

typedef void (*ST_exit_hook)(void);

extern ST_OP_MODE ST_op_mode;

int ST_add_sigint_hook(void (*func)(void));

void ST_demonize(void);

#endif