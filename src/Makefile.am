AUTOMAKE_OPTIONS = subdir-objects
# what flags you want to pass to the C compiler & linker
# -D_FORTIFY_SOURCE=2 -fstack-protector
AM_CFLAGS = --pedantic -Wall -std=gnu99 -O0 -lpthread -lsystemd -lconfig  \
-Wno-unused-result
AM_LDFLAGS =

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
sbin_PROGRAMS = stored
bin_PROGRAMS = storectl
stored_SOURCES = stored.c mtab_check.h mtab_check.c mtab_check_trigger.h mtab_check_trigger.c \
util/logger.h util/logger.c \
util/demonizer.h util/demonizer.c \
util/configure.h util/configure.c \
util/sds.h util/sds.c util/sdsalloc.c
storectl_SOURCES = storectl.c
