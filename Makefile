CC=gcc
CFLAGS=-Wall -Werror -Wextra -std=c11

all: my_cat my_grep

my_cat: my_cat.c my_cat.h
	$(CC) $(CFLAGS) my_cat.c -o my_cat

my_grep: my_grep.c my_grep.h
	$(CC) $(CFLAGS) my_grep.c -o my_grep

tests_my_cat: my_cat
	bash tests_my_cat.sh

tests_my_grep: my_grep
	bash tests_my_grep.sh

clean: 
	rm -rf *.o my_cat my_grep

.PHONY: all tests_my_cat tests_my_grep clean