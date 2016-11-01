/* vim: set tabstop=2 expandtab: */
#ifndef MTAB_CHECK_LOOP_H
#define MTAB_CHECK_LOOP_H

#include "util/configure.h"
#include "util/srv.h"
#include "mtab_checker.h"

void ST_init_check_loop(
        ST_CONFIG c,
        ST_SRV_BUFF b,
        ST_MTAB_ENTRIES me
);

void ST_check_loop(void);

void ST_break_check_loop(void);

#endif