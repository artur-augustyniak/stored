#ifndef DEMONIZER_H
#define DEMONIZER_H

typedef enum _ST_OP_MODE
{
    ST_FORKING,
    ST_NOTIFY
}
ST_OP_MODE;

typedef void (*exit_hook)(void);

extern ST_OP_MODE ST_op_mode;

__BEGIN_DECLS

int ST_add_sigint_hook(void (*func)(void));

void ST_demonize(void);

__END_DECLS
#endif