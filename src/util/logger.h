#ifndef LOGGER_H
#define LOGGER_H

void open_log(const char *name);

void close_log();

void put_notice(char* msg);

void put_error(char* msg);

#endif