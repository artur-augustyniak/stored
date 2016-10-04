/* vim: set tabstop=2 expandtab: */
#include<stdio.h>
#include <stdlib.h>
#include "common.h"
#include "configure.h"

ST_CONFIG core_config = NULL;

/* gcc -g -lconfig testing_main.c configure.c */
int  main(void)
{
    ST_CONFIG c;
    c = ST_new_config("/tmp/stored.cfg");
    core_config = c;
    ST_destroy_config(core_config);
    printf("blah\n");
    return 0;
}
