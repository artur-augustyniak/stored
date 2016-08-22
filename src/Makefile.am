AUTOMAKE_OPTIONS = subdir-objects
# what flags you want to pass to the C compiler & linker
AM_CFLAGS = --pedantic -Wall -std=gnu99 -O3 -lpthread -lsystemd -D_FORTIFY_SOURCE=2 -fstack-protector \
-Wno-unused-result
AM_LDFLAGS =

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
sbin_PROGRAMS = stored
bin_PROGRAMS = storectl
stored_SOURCES = stored.c mtab_check.h mtab_check.c mtab_check_trigger.h mtab_check_trigger.c \
util/logger.h util/logger.c \
util/demonizer.h util/demonizer.c \
srv/srv.h srv/srv.c
storectl_SOURCES = storectl.c
