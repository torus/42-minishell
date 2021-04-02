/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_test.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thisai <thisai@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/02 11:27:21 by thisai            #+#    #+#             */
/*   Updated: 2021/04/02 12:15:58 by thisai           ###   ########.fr       */
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
#define TEST_SECTION(message) printf("- " message "\n")

void	init_buf_with_string(t_parse_buffer *buf, const char* str)
{
    buf->cur_pos = 0;
    strcpy(buf->buffer, str);
    buf->size = strlen(str);
}

int main()
{
    TEST_SECTION("parse_getc");
    {
        t_parse_buffer buf;
        init_buf_with_string(&buf, "abcde");
        int x = parse_getc(&buf);
        CHECK_EQ(x, 'a');
        x = parse_getc(&buf);
        CHECK_EQ(x, 'b');
    }

    TEST_SECTION("parse_read_word(buf, result)");
    {
        t_parse_buffer	buf;
        init_buf_with_string(&buf, "cat ");
        t_parse_token	tok;
        parse_read_word(&buf, &tok);
        CHECK_EQ(tok.length, 3);
        CHECK(!strncmp(tok.text, "cat", 3));
    }

    TEST_SECTION("parse_get_token");
    {
        t_parse_buffer	buf;
        init_buf_with_string(&buf, "cat $ABC | grep x | wc > file.txt\n");
        t_parse_token	tok;
        parse_get_token(&buf, &tok);
        CHECK_EQ(tok.type, TOKTYPE_EXPANDABLE);
        printf("tok.length = %d, tok.text = %s\n", tok.length, tok.text);
        CHECK_EQ(tok.length, 3);
        CHECK(!strncmp(tok.text, "cat", 3));
    }

	print_result();
}
