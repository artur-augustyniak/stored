#ifndef MTAB_CHECK_H
#define MTAB_CHECK_H
#include<pthread.h>

  typedef struct NOTICED_ENTRY{
    char *path;
    int free_percent;
  } NOTICED_ENTRY, *NE;

  int runtime_entries_capacity;
  NE *entries;
  int entries_count;
  pthread_mutex_t entries_lock;

  void init_mtab(void);
  void destroy_mtab(void);
  void check_mtab(void);
  void report_list(FILE *stream);
  void destory_current_notices(void);

#endif