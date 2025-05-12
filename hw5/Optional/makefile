# Variables
# variables for compiling rules
SHELL=/bin/bash
CC=gcc
CFLAGS=-Wall -pedantic -std=c17 -g3 

# paths for testing/submitting
HW5=/c/cs223/hw5
BIN=/c/cs223/bin


# Compiling

all: Unit

Unit: kdtree.o location.o kdtree_unit.o
	${CC} ${CCFLAGS} -o $@ $^ -lm

kdtree.o: kdtree.h location.h kdtree_helpers.h kdtree_internal.h
location.o: location.h
kdtree_unit.o: kdtree.h location.h


clean:
	rm -f Unit *.o


test:
	${HW4}/Tests/test.Unit


singletest:
	${HW5}/Tests/$(TEST)


submit:
	${BIN}/submit 5 makefile kdtree.c kdtree_helpers.c kdtree_helpers.h kdtree_internal.h log

check:
	${BIN}/check 5

testit:
	${BIN}/testit 5 Unit

