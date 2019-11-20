#
# Simple Makefile for compiling and git interface
# author: Raimund Kirner
# 
SHELL = bash
#GCC = arm-linux-gnueabi-gcc  # cross-compilation for RPI on Linux
GCC = gcc
LIB = initio
DEFINE = -D HAVE_PIROCON2   #possible roboboard definitions: HAVE_ROBOHAT, HAVE_PIROCON2


.PHONY: all compile link install status pull commit sync help

all: status

$(LIB).o: $(LIB).c
	$(GCC) -c -Wall -Werror -I./resources $(LIB).c

lib$(LIB).so: $(LIB).o
	$(GCC) -shared -o lib$(LIB).so $(LIB).o

compile:
	$(GCC) -c -Wall -Werror -I./resources $(DEFINE) $(LIB).c

link:
	$(GCC) -shared -o lib$(LIB).so $(LIB).o

install: lib$(LIB).so
	sudo cp $(LIB).h /usr/local/include/$(LIB).h
	sudo cp lib$(LIB).so /usr/local/lib/lib$(LIB).so
	@echo "installation done. Please make sure that 'servod' is within search path."

status:
	git status

pull:
	git pull

commit:
	git commit
	git push

sync: pull commit

clean:
	rm -f $(LIB).o lib$(LIB).so

help:
	@echo
	@echo "Possible commands:"
	@echo " > make compile"
	@echo " > make link"
	@echo " > make install"
	@echo " > make status"
	@echo " > make pull"
	@echo " > make commit"
	@echo " > make sync"
	@echo " > make clean"
	@echo

