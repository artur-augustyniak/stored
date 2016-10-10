#ifndef MTAB_CHECK_TRIGGER_H
#define MTAB_CHECK_TRIGGER_H
#include "util/configure.h"
#include "util/srv.h"

void ST_init_checks_loop(ST_CONFIG conf, ST_SRV_BUFF buff);

void ST_checks_loop(void (*check_func)(void), int timeout);

void ST_break_checks_loop(void);

#endif