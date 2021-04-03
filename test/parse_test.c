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
		printf("  ✔ %s\n", msg);
	}
	else
	{
		fail_count++;
		printf("  ✖ %s\n", msg);
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

    TEST_SECTION("parse_get_token クォートなしの場合");
    {
        t_parse_buffer	buf;
        init_buf_with_string(&buf, "cat $ABC |wc> file.txt\n");
        t_parse_token	tok;

        parse_get_token(&buf, &tok);
        CHECK_EQ(tok.type, TOKTYPE_EXPANDABLE);
        CHECK_EQ(tok.length, 3);
        CHECK(!strncmp(tok.text, "cat", 3));

        parse_get_token(&buf, &tok);
        CHECK_EQ(tok.type, TOKTYPE_SPACE);

        parse_get_token(&buf, &tok);
        CHECK_EQ(tok.type, TOKTYPE_EXPANDABLE);
        CHECK_EQ(tok.length, 4);
        CHECK(!strncmp(tok.text, "$ABC", 4));

        parse_get_token(&buf, &tok);
        CHECK_EQ(tok.type, TOKTYPE_SPACE);

        parse_get_token(&buf, &tok);
        CHECK_EQ(tok.type, TOKTYPE_PIPE);

        parse_get_token(&buf, &tok);
        CHECK_EQ(tok.type, TOKTYPE_EXPANDABLE);
        CHECK_EQ(tok.length, 2);
        CHECK(!strncmp(tok.text, "wc", 2));

        parse_get_token(&buf, &tok);
        CHECK_EQ(tok.type, TOKTYPE_OUTPUT_REDIRECTION);
        parse_get_token(&buf, &tok);
        CHECK_EQ(tok.type, TOKTYPE_SPACE);

        parse_get_token(&buf, &tok);
        CHECK_EQ(tok.type, TOKTYPE_EXPANDABLE);
        CHECK_EQ(tok.length, 8);
        CHECK(!strncmp(tok.text, "file.txt", 8));

        parse_get_token(&buf, &tok);
        CHECK_EQ(tok.type, TOKTYPE_NEWLINE);
    }

    TEST_SECTION("parse_get_token クォートありの場合");
    {
        t_parse_buffer	buf;
        init_buf_with_string(&buf, "cat\"$ABC\"'wc'");
        t_parse_token	tok;

        parse_get_token(&buf, &tok);
        CHECK_EQ(tok.type, TOKTYPE_EXPANDABLE);
        CHECK_EQ(tok.length, 3);
        CHECK(!strncmp(tok.text, "cat", 3));

        parse_get_token(&buf, &tok);
        CHECK_EQ(tok.type, TOKTYPE_EXPANDABLE);
        CHECK_EQ(tok.length, 4);
        CHECK(!strncmp(tok.text, "$ABC", 4));

        parse_get_token(&buf, &tok);
        CHECK_EQ(tok.type, TOKTYPE_NON_EXPANDABLE);
        CHECK_EQ(tok.length, 2);
        CHECK(!strncmp(tok.text, "wc", 2));
    }

    TEST_SECTION("parse_get_token のこり");
    {
        t_parse_buffer	buf;
        init_buf_with_string(&buf, "cat < - ; ");
        t_parse_token	tok;

        parse_get_token(&buf, &tok);
        CHECK_EQ(tok.type, TOKTYPE_EXPANDABLE);
        CHECK_EQ(tok.length, 3);
        CHECK(!strncmp(tok.text, "cat", 3));

        parse_get_token(&buf, &tok);
        CHECK_EQ(tok.type, TOKTYPE_SPACE);

        parse_get_token(&buf, &tok);
        CHECK_EQ(tok.type, TOKTYPE_INPUT_REDIRECTION);

        parse_get_token(&buf, &tok);
        CHECK_EQ(tok.type, TOKTYPE_SPACE);

        parse_get_token(&buf, &tok);
        CHECK_EQ(tok.type, TOKTYPE_EXPANDABLE);
        CHECK_EQ(tok.length, 1);
        CHECK(!strncmp(tok.text, "-", 1));

        parse_get_token(&buf, &tok);
        CHECK_EQ(tok.type, TOKTYPE_SPACE);

        parse_get_token(&buf, &tok);
        CHECK_EQ(tok.type, TOKTYPE_SEMICOLON);
    }

	print_result();
}
