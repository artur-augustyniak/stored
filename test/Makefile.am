# make check

AUTOMAKE_OPTIONS = subdir-objects
# what flags you want to pass to the C compiler & linker
AM_CFLAGS = --pedantic -Wall -std=gnu99 -O0 -g -lcunit
AM_LDFLAGS =

# SCRIPTS
TESTS = \
    static/flawfind.sh \
    misc/dummy.sh

# COMPILE on make check
check_PROGRAMS = util_logger_test
util_logger_test_SOURCES =  \
    util/logger_test.c      \
    ../src/util/logger.c    \
    ../src/util/logger.h

TESTS += $(check_PROGRAMS)