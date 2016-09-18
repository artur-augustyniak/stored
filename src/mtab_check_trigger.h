#ifndef MTAB_CHECK_TRIGGER_H
#define MTAB_CHECK_TRIGGER_H
#include "util/configure.h"

void ST_init_checks_loop(ST_conf conf);

void ST_checks_loop(void (*check_func)(void), int timeout);

void ST_break_checks_loop(void);

#endif