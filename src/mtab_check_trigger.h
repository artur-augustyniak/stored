#ifndef MTAB_CHECK_TRIGGER_H
#define MTAB_CHECK_TRIGGER_H

void ST_checks_loop(void (*check_func)(void));

void ST_break_checks_loop(void);

#endif