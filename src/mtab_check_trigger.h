#ifndef MTAB_CHECK_TRIGGER_H
#define MTAB_CHECK_TRIGGER_H

extern int ST_timeout;

void ST_checks_loop(void (*check_func)(void));

void ST_break_checks_loop(void);

#endif