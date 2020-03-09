# what flags you want to pass to the C compiler & linker
AM_CXXFLAGS = -std=c++17 -pthread -O2
AM_LDFLAGS =
# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
SUBDIRS = function dispatch statistics
bin_PROGRAMS = main

# .
main_SOURCES = main.cpp define.h nvwa/debug_new.h #$(AR) $(ARFLAGS)

main_LDADD = $(top_srcdir)/src/dispatch/libdispatch.a \
$(top_srcdir)/src/function/libfunction.a \
$(top_srcdir)/src/statistics/libstatistics.a


