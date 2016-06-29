#
# Simple Makefile for svn interface
#
SHELL = bash
GCC = gcc
PROG1 = initioTest
PROG2 = initioTest2

.PHONY: all status update commit sync help

all: $(PROG1) $(PROG2)

run: $(PROG2)
	sudo ./$<

$(PROG1): $(PROG1).c
	$(GCC) -o $@ -Wall -Werror -lcurses -lwiringPi -lpthread -linitio -I.. $<

$(PROG2): $(PROG2).c
	$(GCC) -o $@ -Wall -Werror -lcurses -lwiringPi -lpthread -linitio -I.. $<

status:
	svn status

update:
	svn update

commit:
	svn commit -m "Update by $(USER)..."

sync: update commit

help:
	@echo
	@echo "Possible commands:"
	@echo " > make status"
	@echo " > make update"
	@echo " > make commit"
	@echo " > make sync"
	@echo

