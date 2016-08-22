#ifndef LOGGER_H
#define LOGGER_H

/*
 * Msg. types
 */
#define ST_MSG_PLAIN   0
#define ST_MSG_NOTICE  1
#define ST_MSG_WARN    2
#define ST_MSG_ERROR   3
#define ST_MSG_CRIT    4

typedef enum _ST_SINK {ST_SYSLOG, ST_STDOUT} ST_SINK;
extern ST_SINK ST_sink_type;

__BEGIN_DECLS
void ST_msg(char* msg, int type);
__END_DECLS

#endif