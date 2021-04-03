run: parse_test
	./parse_test

parse_test: parse_test.c Makefile ../parse.c ../parse2.c ../parse.h
	gcc -g -o $@ parse_test.c
	gcc -Wall -Wextra -g -o $@ parse_test.c

