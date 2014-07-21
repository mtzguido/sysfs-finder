.PHONY:clean all
CFLAGS=-Wall -Wextra -O99
SHELL=/bin/bash
NAME=find

all: find

%: %.c 
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f find
