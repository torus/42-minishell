LIBFT_PATH = ../libft
LIBFT_MAKE = $(MAKE) -C $(LIBFT_PATH)
LIBFT_LIB = ../libft/libft.a

run: parse_test env_test
	./parse_test
	./env_test

parse_test: Makefile parse_test.c test.c test.h ../parse.c ../parse2.c ../parse.h
	gcc -Wall -Wextra -g -o $@ parse_test.c test.c ../parse.c ../parse2.c

env_test: env_test.c test.c test.h ../env.c ../env.h
	$(LIBFT_MAKE)
	gcc -Wall -Wextra -g -o $@ $^ ${LIBFT_LIB}
