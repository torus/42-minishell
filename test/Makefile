run: parse_test
	./parse_test

SRCS_PARSE = $(wildcard ../parse*.c) ../libft/ft_memcpy.c

parse_test: Makefile parse_test.c test.c test.h $(SRCS_PARSE) ../parse.h
	gcc -Wall -Wextra -g -o $@ parse_test.c test.c $(SRCS_PARSE)

norm:
	docker-compose run --rm norm norminette ../*.[ch]
