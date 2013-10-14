
CC= gcc
FLAGS= -c -g

SOURCES=$(wildcard *.c)
EXECUTABLES=$(SOURCES:%.c=%)
all: $(EXECUTABLES)


clean:
	rm -f $(EXECUTABLES)


