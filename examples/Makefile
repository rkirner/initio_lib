#
# Simple Makefile for compiling programs for initio robot car and git interface
#
SHELL	= bash
GCC	= gcc
CFLAGS	= -Wall -Werror
LFLAGS	= -linitio -lwiringPi -lcurses -lpthread

PROGS 	= testIR \
	  testIO \
	  remoteControl \
	  remoteControl2 \

RUN	= remoteControl2

.PHONY: all run status pull commit sync help

all: $(PROGS)

run: $(RUN)
	sudo ./$<

% : %.c
	$(GCC) -o $@ $(CFLAGS) $(LFLAGS) $<

clean:
	rm $(PROGS)

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
	@echo " > make clean"
	@echo " > make status"
	@echo " > make pull"
	@echo " > make commit"
	@echo " > make sync"
	@echo

