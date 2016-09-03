#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <CUnit/Basic.h>
#include "../src/util/logger.h"
#include "clib_mock.h"

#define TMP_EXCHANGE_FILE  "logger_suite.tmp"

#define TEST_MSG  "Blah"
#define OUTPUT_TEST_MSG  "Blah"

#define STRINGIZE(A) #A
#define OUTPUT_TEST_MSG_WITH_TYPE(prio) "<"STRINGIZE(prio)"> " OUTPUT_TEST_MSG

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

int init_logger_suite(void)
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

int clean_logger_suite(void)
{
   if (0 != fclose(temp_file)) {
      return -1;
   }
   else {
      unlink(TMP_EXCHANGE_FILE);
      return 0;
   }
}

void
test_default_sink_type_is_stdout
(void)
{
    CU_ASSERT(ST_STDOUT == ST_sink_type);
}

void
test_ST_msg_sink_type_is_stdout_produce_output_to_stdout
(void)
{
    size_t test_buff_size = 10;
    char buffer[test_buff_size];
    char expected_msg[] = OUTPUT_TEST_MSG_WITH_TYPE(ST_MSG_PLAIN);
    size_t expected_len = strlen(expected_msg);

    start_stdout_redir();
    /* interaction */
    ST_logger_msg(TEST_MSG, ST_MSG_PLAIN);
    stop_stdout_redir();

    rewind(temp_file);
    CU_ASSERT(expected_len + 1 == fread(
        buffer,
        sizeof(unsigned char),
        test_buff_size,
        temp_file)
    );
    CU_ASSERT(0 == strncmp(buffer, expected_msg, expected_len));
}

void
test_ST_msg_sink_type_is_syslog_call_openlog_at_first_call
(void)
{
    int call_times = 10;

    ST_sink_type = ST_SYSLOG;
    for(int i = 0 ; i < call_times ; i++)
    {
        ST_logger_msg(TEST_MSG, ST_MSG_PLAIN);
    }

    CU_ASSERT(1 == openlog_call_counter);
    CU_ASSERT(1 == closelog_call_counter);
    CU_ASSERT(1 == atexit_call_counter);
    CU_ASSERT(call_times == syslog_call_counter);
}



int main()
{

    if (CUE_SUCCESS != CU_initialize_registry())
    {
        return CU_get_error();
    }

    CU_pSuite suite_handler = CU_add_suite(
        "Logger Suite",
        init_logger_suite,
        clean_logger_suite
    );

    if (NULL == suite_handler)
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

   if( (NULL == CU_add_test(
            suite_handler,
            "test_default_sink_type_is_stdout",
             test_default_sink_type_is_stdout)
        )
        || (NULL == CU_add_test(
            suite_handler,
            "test_ST_msg_sink_type_is_stdout_produce_output_to_stdout",
            test_ST_msg_sink_type_is_stdout_produce_output_to_stdout
        ))
        || (NULL == CU_add_test(
            suite_handler,
            "test_ST_msg_sink_type_is_syslog_call_openlog_at_first_call",
            test_ST_msg_sink_type_is_syslog_call_openlog_at_first_call
        ))
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
