#ifndef MTAB_CHECK_TRIGGER_H
#define MTAB_CHECK_TRIGGER_H

void init_checks_loop(void);

void break_checks_loop(void);

void checks_loop(void (*check_func)(void));

#endif