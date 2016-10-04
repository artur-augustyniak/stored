/* vim: set tabstop=2 expandtab: */
#ifndef COMMON_H
#define COMMON_H

#define ERR_STATUS  1
#define ERR_NUM     0

void ST_abort(const char *file, unsigned int line, const char *msg)
{
    error(ERR_STATUS, ERR_NUM, "file: %s:%d - %s",file, line, msg );
    abort();
}

#endif