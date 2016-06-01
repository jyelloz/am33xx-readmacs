-include config.mk

CFLAGS += -std=gnu11 -pedantic -Wall -Werror

AS		= $(CROSS_COMPILE)as
LD		= $(CROSS_COMPILE)ld
CC		= $(CROSS_COMPILE)gcc
AR		= $(CROSS_COMPILE)ar
NM		= $(CROSS_COMPILE)nm
STRIP	= $(CROSS_COMPILE)strip
OBJCOPY	= $(CROSS_COMPILE)objcopy
OBJDUMP	= $(CROSS_COMPILE)objdump
INSTALL = install

PREFIX ?= /usr
BINDIR = $(DESTDIR)$(PREFIX)/bin

PROGS = am33xx-readmacs

all: $(PROGS) Makefile config.mk

install: $(PROGS)
	$(INSTALL) -d $(BINDIR)
	$(INSTALL) -m 0755 $(PROGS) $(BINDIR)

clean: Makefile
	rm -f $(PROGS)
