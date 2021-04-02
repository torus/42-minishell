/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_test.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thisai <thisai@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/02 11:27:21 by thisai            #+#    #+#             */
/*   Updated: 2021/04/02 11:53:59 by thisai           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "../parse.c"

int		success_count = 0;
int		fail_count = 0;

void	check(int64_t val, const char *msg)
{
	if (val)
	{
		success_count++;
		printf("✔ %s\n", msg);
	}
	else
	{
		fail_count++;
		printf("✖ %s\n", msg);
	}
}

void	print_result()
{
	printf("✔: %d ✖: %d\n", success_count, fail_count);
}

#define CHECK(val) check((int64_t)val, #val)
#define CHECK_EQ(actual, expected) check(actual == expected, #actual " == " #expected)

int main()
{
	t_parse_buffer buf;
	buf.cur_pos = 0;

	strcpy(buf.buffer, "abcde");
	buf.size = strlen("abcde");
	int x = parse_getc(&buf);
	CHECK_EQ(x, 'a');


	print_result();
}
