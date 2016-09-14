#ifndef MTAB_CHECK_H
#define MTAB_CHECK_H
#include "util/configure.h"

void ST_init_check_mtab(ST_conf conf);

void ST_check_mtab(void);

void ST_report_list(FILE *stream);

void ST_destroy_check_mtab(void);

#endif