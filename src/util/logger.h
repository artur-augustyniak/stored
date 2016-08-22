#ifndef LOGGER_H
#define LOGGER_H

/*
 * Msg. types
 */
#define MSG_PLAIN   0
#define MSG_NOTICE  1
#define MSG_WARN    2
#define MSG_ERROR   3
#define MSG_CRIT    4

__BEGIN_DECLS
void msg(char* msg, int type);
__END_DECLS

#endif