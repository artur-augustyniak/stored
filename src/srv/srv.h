#ifndef SRV_H
#define SRV_H

extern int ST_enabled;
extern int ST_port;
extern const char *ST_bind_addr;

void ST_start_server(void);

void ST_stop_server(void);

#endif