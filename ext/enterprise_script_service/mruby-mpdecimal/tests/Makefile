SRCDIR = ../src

CC = gcc
LD = gcc
AR = ar

MPD_WARN = -Wall -W -Wno-unknown-pragmas -std=c99 -pedantic
MPD_CONFIG = -DCONFIG_64 -DASM

CONFIGURE_CFLAGS = -ggdb -Wall -W -Wno-unknown-pragmas -std=c99 -pedantic -DCONFIG_64 -DASM -O0
CFLAGS ?= $(CONFIGURE_CFLAGS)

HEADERS = $(wildcard $(SRCDIR)/*.h)
SOURCES = $(filter-out $(SRCDIR)/ext.c, $(wildcard $(SRCDIR)/*.c))

default: runtest runtest_alloc

# Short test.
runtest: Makefile runtest.c malloc_fail.c malloc_fail.h mptest.h $(HEADERS) $(SOURCES)
	$(CC) -I$(SRCDIR) $(CFLAGS) -o runtest runtest.c malloc_fail.c $(SOURCES) -lm

# Also test all allocation failures.
runtest_alloc: Makefile runtest.c malloc_fail.c malloc_fail.h mptest.h $(HEADERS) $(SOURCES)
	$(CC) -I$(SRCDIR) $(CFLAGS) -DTEST_ALLOC -o runtest_alloc runtest.c malloc_fail.c $(SOURCES) -lm

FORCE:

clean: FORCE
	rm -f *.o *.gch *.gcda *.gcno *.gcov *.dyn *.dpi *.lock
	rm -f runtest runtest_shared runtest_alloc runtest_alloc_shared

distclean: FORCE
	$(MAKE) clean
	rm -rf dectest.zip testdata
