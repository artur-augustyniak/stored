# make check

AUTOMAKE_OPTIONS = subdir-objects
# what flags you want to pass to the C compiler & linker
AM_CFLAGS = --pedantic -Wall -std=gnu99 -O0 -g -lcunit -lsystemd
AM_LDFLAGS =

# SCRIPTS
TESTS = \
    static/flawfind.sh \
    misc/dummy.sh

# COMPILE on make check
check_PROGRAMS = util_logger_test util_demonizer_test
util_logger_test_SOURCES =  \
    util/logger_test.c      \
    ../src/util/logger.c    \
    ../src/util/logger.h    \
    ../src/util/sds.c    \
     ../src/util/sds.h

util_demonizer_test_SOURCES =  \
    util/demonizer_test.c      \
    ../src/util/demonizer.c    \
    ../src/util/demonizer.h

TESTS += $(check_PROGRAMS)