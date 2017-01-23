DESTDIR ?=
PREFIX ?= /usr/local
BINDIR ?= $(PREFIX)/bin
MANDIR ?= $(PREFIX)/man

CROSS_COMPILE ?=
CC = $(CROSS_COMPILE)gcc

CFLAGS += -Wall -Wextra -Werror -Wfatal-errors
CFLAGS += -I ../include -g
CFLAGS += -L ../src
CFLAGS += -O2
LDFLAGS += -ltinyalsa -lexpat

VPATH = ../src:../include/tinyroute

.PHONY: all
all: -ltinyroute tinyroute 

tinyroute: tinyroute.c audio_route.h libtinyroute.a 

.PHONY: clean
clean:
	rm -f tinyroute

.PHONY: install
install: tinyroute
	install -d $(DESTDIR)$(BINDIR)
	install tinyroute $(DESTDIR)$(BINDIR)/
	install -d $(DESTDIR)$(MANDIR)/man1
	install tinyroute.1 $(DESTDIR)$(MANDIR)/man1/

