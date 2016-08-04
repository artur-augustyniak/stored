# what flags you want to pass to the C compiler & linker
CFLAGS = --pedantic -Wall -std=c99 -O3
LDFLAGS =

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
bin_PROGRAMS = stored storectl
stored_SOURCES = stored.c mtab_check.h mtab_check.c mtab_check_trigger.h mtab_check_trigger.c
storectl_SOURCES = storectl.c
