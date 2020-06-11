# what flags you want to pass to the C compiler & linker
AM_CXXFLAGS = -std=c++17 -pthread
AM_LDFLAGS =
# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
SUBDIRS = function dispatch statistics
bin_PROGRAMS = main

# .
main_SOURCES = main.cpp define.h nvwa/nvwa/debug_new.h nvwa/nvwa/debug_new.cpp

main_LDADD = dispatch/libdispatch.a \
function/libfunction.a \
statistics/libstatistics.a


