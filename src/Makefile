DESTDIR ?=
PREFIX ?= /usr/local
LIBDIR ?= $(PREFIX)/lib
BINDIR ?= $(PREFIX)/bin
ifdef DEB_HOST_MULTIARCH
LIBDIR := $(LIBDIR)/$(DEB_HOST_MULTIARCH)
endif

CROSS_COMPILE =
CC = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar
LD = $(CROSS_COMPILE)gcc

WARNINGS = -Wall -Wextra -Werror -Wfatal-errors -g
INCLUDE_DIRS = -I ../include
override CFLAGS := $(WARNINGS) $(INCLUDE_DIRS) -fPIC $(CFLAGS)

VPATH = ../include/tinyroute
OBJECTS = audio_route.o

.PHONY: all
all: libtinyroute.a libtinyroute.so

audio_route.o: audio_route.c audio_route.h

libtinyroute.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $^

libtinyroute.so: libtinyroute.so.1
	ln -sf $< $@

libtinyroute.so.1: libtinyroute.so.1.1.0
	ln -sf $< $@

libtinyroute.so.1.1.0: $(OBJECTS)
	$(LD) $(LDFLAGS) -shared -Wl,-soname,libtinyroute.so.1 $^ -o $@

.PHONY: clean
clean:
	rm -f libtinyroute.a
	rm -f libtinyroute.so
	rm -f libtinyroute.so.1
	rm -f libtinyroute.so.1.1.0
	rm -f $(OBJECTS)

.PHONY: install
install: libtinyroute.a libtinyroute.so.1
	install -d $(DESTDIR)$(LIBDIR)/
	install libtinyroute.a $(DESTDIR)$(LIBDIR)/
	install libtinyroute.so.1.1.0 $(DESTDIR)$(LIBDIR)/
	ln -sf libtinyroute.so.1.1.0 $(DESTDIR)$(LIBDIR)/libtinyroute.so.1
	ln -sf libtinyroute.so.1 $(DESTDIR)$(LIBDIR)/libtinyroute.so

