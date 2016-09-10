#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <CUnit/Basic.h>
#include <signal.h>
#include "../src/util/demonizer.h"
#include "clib_mock.h"

static int msg_type;

void ST_logger_msg(char* msg, int type)
{
    msg_type = type;
}

#define TMP_EXCHANGE_FILE  "demonizer_suite.tmp"

FILE *temp_file;

static int bak_fd, new_fd;

static void start_stdout_redir(void)
{
    fflush(stdout);
    bak_fd = dup(1);
    new_fd = open(TMP_EXCHANGE_FILE, O_WRONLY);
    dup2(new_fd, 1);
    close(new_fd);
}

static void stop_stdout_redir(void){
    fflush(stdout);
    dup2(bak_fd, 1);
    close(bak_fd);
}

int init_suite(void)
{
    if (NULL == (temp_file = fopen(TMP_EXCHANGE_FILE, "w+")))
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

int clean_suite(void)
{
   if (0 != fclose(temp_file)) {
      return -1;
   }
   else {
      //unlink(TMP_EXCHANGE_FILE);
      return 0;
   }
}



void
test_wo_init_demonize_log_error
(void)
{
    ST_demonize();
    CU_ASSERT(1234 == msg_type);
}

int main()
{

    if (CUE_SUCCESS != CU_initialize_registry())
    {
        return CU_get_error();
    }

    CU_pSuite suite_handler = CU_add_suite(
        "Demonizer Suite",
        init_suite,
        clean_suite
    );

    if (NULL == suite_handler)
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

   if( (NULL == CU_add_test(
            suite_handler,
            "test_wo_init_demonize_log_error",
             test_wo_init_demonize_log_error)
        )
   )
   {
      CU_cleanup_registry();
      return CU_get_error();
   }


   CU_basic_set_mode(CU_BRM_VERBOSE);
   CU_basic_run_tests();

   int exit_code = CU_get_number_of_tests_failed();

   CU_cleanup_registry();
   return exit_code + CU_get_error();
}
