run: parse_test
	./parse_test

parse_test: Makefile parse_test.c test.c test.h ../parse.c ../parse2.c ../parse.h
	gcc -Wall -Wextra -g -o $@ parse_test.c test.c ../parse.c ../parse2.c

norm:
	docker-compose run --rm norm norminette ../*.[ch]
