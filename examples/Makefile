#
# Simple Makefile for compiling programs for initio robot car and git interface
#
SHELL	= bash
GCC	= gcc
LIBS	= -lcurses -lwiringPi -lpthread -linitio
CFLAGS	= -Wall -Werror

PROGS 	= testIR \
	  remoteControl \
	  remoteControl2 \

RUN	= remoteControl2

.PHONY: all run status pull commit sync help

all: $(PROGS)

run: $(RUN)
	sudo ./$<

% : %.c
	$(GCC) -o $@ $(CFLAGS) $(LIBS) $<

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

