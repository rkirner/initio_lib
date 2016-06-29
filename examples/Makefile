#
# Simple Makefile for compiling and git interface
#
SHELL = bash
GCC = gcc
PROG1 = initioTest
PROG2 = initioTest2

.PHONY: all run status pull commit sync help

all: $(PROG1) $(PROG2)

run: $(PROG2)
	sudo ./$<

$(PROG1): $(PROG1).c
	$(GCC) -o $@ -Wall -Werror -lcurses -lwiringPi -lpthread -linitio -I.. $<

$(PROG2): $(PROG2).c
	$(GCC) -o $@ -Wall -Werror -lcurses -lwiringPi -lpthread -linitio -I.. $<

status:
	git status

pull:
	git pull

commit:
	git commit
	git push

sync: pull commit

help:
	@echo
	@echo "Possible commands:"
	@echo " > make run"
	@echo " > make status"
	@echo " > make pull"
	@echo " > make commit"
	@echo " > make sync"
	@echo

