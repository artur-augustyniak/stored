#ifndef MTAB_CHECK_H
#define MTAB_CHECK_H

#define INIT_CAPACITY 20

  typedef struct NOTICED_ENTRY{
    char *path;
    int free_percent;
    struct NOTICED_ENTRY *next;
  } NOTICED_ENTRY, *NE;

  NE entries_handle;
  NE last_slot;


  void destory_current_notices(void);

  void init_mtab(void);

  void destroy_mtab(void);

  void check_mtab(void);

#endif