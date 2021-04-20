#include "test.h"
#include "../parse.h"

void	init_buf_with_string(t_parse_buffer *buf, const char* str)
{
	buf->cur_pos = 0;
	strcpy(buf->buffer, str);
	buf->size = strlen(str);
}

void test_lexer()
{
	TEST_CHAPTER("レキサー");

	TEST_SECTION("lex_getc");
	{
		t_parse_buffer buf;
		init_buf_with_string(&buf, "abcde");
		int x = lex_getc(&buf);
		CHECK_EQ(x, 'a');
		x = lex_getc(&buf);
		CHECK_EQ(x, 'b');
	}

	TEST_SECTION("lex_read_word(buf, result)");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "cat ");
		t_token	tok;
		lex_read_word(&buf, &tok);
		CHECK_EQ(tok.length, 3);
		CHECK(!strncmp(tok.text, "cat", 3));
	}

	TEST_SECTION("lex_get_token クォートなしの場合");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "cat $ABC |wc> file.txt\n");
		t_token	tok;

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_EXPANDABLE);
		CHECK_EQ(tok.length, 3);
		CHECK(!strncmp(tok.text, "cat", 3));

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_SPACE);

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_EXPANDABLE);
		CHECK_EQ(tok.length, 4);
		CHECK(!strncmp(tok.text, "$ABC", 4));

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_SPACE);

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_PIPE);

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_EXPANDABLE);
		CHECK_EQ(tok.length, 2);
		CHECK(!strncmp(tok.text, "wc", 2));

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_OUTPUT_REDIRECTION);
		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_SPACE);

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_EXPANDABLE);
		CHECK_EQ(tok.length, 8);
		CHECK(!strncmp(tok.text, "file.txt", 8));

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_NEWLINE);
	}

	TEST_SECTION("lex_get_token クォートありの場合");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "cat\"$ABC\"'wc'");
		t_token	tok;

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_EXPANDABLE);
		CHECK_EQ(tok.length, 3);
		CHECK(!strncmp(tok.text, "cat", 3));

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_EXPANDABLE_QUOTED);
		CHECK_EQ(tok.length, 4);
		CHECK(!strncmp(tok.text, "$ABC", 4));

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_NON_EXPANDABLE);
		CHECK_EQ(tok.length, 2);
		CHECK(!strncmp(tok.text, "wc", 2));
	}

	TEST_SECTION("lex_get_token のこり");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "cat < - ; ");
		t_token	tok;

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_EXPANDABLE);
		CHECK_EQ(tok.length, 3);
		CHECK(!strncmp(tok.text, "cat", 3));

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_SPACE);

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_INPUT_REDIRECTION);

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_SPACE);

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_EXPANDABLE);
		CHECK_EQ(tok.length, 1);
		CHECK(!strncmp(tok.text, "-", 1));

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_SPACE);

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_SEMICOLON);
	}
}

void check_string(t_parse_ast *node, const char *expected)
{
	CHECK_EQ(node->type, ASTNODE_STRING);
	CHECK_EQ_STR(node->content.string->text, expected);
}

void check_single_argument(t_parse_ast *node, const char *expected)
{
	t_parse_ast *str_node = node->content.arguments->string_node;
	CHECK(str_node);
	check_string(str_node, expected);
}

void check_args(t_parse_ast	*node)
{
	CHECK(node);
	CHECK_EQ(node->type, ASTNODE_ARGUMENTS);

	check_single_argument(node, "file");

	node = node->content.arguments->rest_node;
	t_parse_ast *red_node = node->content.arguments->redirection_node;
	CHECK(red_node);
	CHECK_EQ(red_node->type, ASTNODE_REDIRECTION);
	CHECK_EQ(red_node->content.redirection->type, TOKTYPE_INPUT_REDIRECTION);
	check_string(red_node->content.redirection->string_node, "abc");
}

void check_piped_commands(t_parse_ast *node)
{
	CHECK_EQ(node->type, ASTNODE_PIPED_COMMANDS);
	CHECK(node->content.piped_commands->command_node);
	CHECK(node->content.piped_commands->next);
	t_parse_ast *next = node->content.piped_commands->next;
	CHECK_EQ(next->type, ASTNODE_PIPED_COMMANDS);
	CHECK(next->content.piped_commands->command_node);
	check_args(next->content.piped_commands->command_node
			  ->content.command->arguments_node);
	CHECK(!next->content.piped_commands->next);
}

void check_delimiter(t_parse_ast *node)
{
	CHECK_EQ(node->type, ASTNODE_DELIMITER);
	CHECK_EQ(node->content.delimiter->type, TOKTYPE_SEMICOLON);
}

void check_piped_seqence(t_parse_ast *node)
{
	check_piped_commands(node->content.sequential_commands->pipcmd_node);
	check_delimiter(
		node->content.sequential_commands
		->delimiter_node);
	check_single_argument(
		node->content.sequential_commands
		->rest_node->content.sequential_commands
		->pipcmd_node->content.piped_commands
		->command_node->content.command
		->arguments_node,
		"xyz");
}

void test_parser(void)
{
	TEST_CHAPTER("パーサ");

	TEST_SECTION("parse_string 単純な文字列");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "file\n");
		t_token	tok;

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_string(&buf, &tok);
		CHECK(node);
		CHECK_EQ(node->type, ASTNODE_STRING);
		CHECK_EQ(node->content.string->type, TOKTYPE_EXPANDABLE);
		CHECK_EQ_STR(node->content.string->text, "file");
		CHECK_EQ(node->content.string->next, NULL);
	}

	TEST_SECTION("parse_redirection　<");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "< file\n");
		t_token	tok;

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_redirection(&buf, &tok);
		CHECK(node);
		CHECK_EQ(node->type, ASTNODE_REDIRECTION);
		CHECK_EQ(node->content.redirection->type, TOKTYPE_INPUT_REDIRECTION);
		CHECK_EQ_STR(node->content.redirection->string_node
					->content.string->text, "file");
	}

	TEST_SECTION("parse_arguments 1 個");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "abc \n");
		t_token	tok;

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_arguments(&buf, &tok);
		CHECK(node);
		CHECK_EQ(node->type, ASTNODE_ARGUMENTS);
		t_parse_ast *str_node = node->content.arguments->string_node;
		CHECK(str_node);
		CHECK_EQ(str_node->type, ASTNODE_STRING);
		CHECK_EQ_STR(str_node->content.string->text, "abc");
	}

	TEST_SECTION("parse_arguments 1 個直後に改行");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "abc\n");
		t_token	tok;

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_arguments(&buf, &tok);
		CHECK(node);
		CHECK_EQ(node->type, ASTNODE_ARGUMENTS);
		t_parse_ast *str_node = node->content.arguments->string_node;
		CHECK(str_node);
		CHECK_EQ(str_node->type, ASTNODE_STRING);
		CHECK_EQ_STR(str_node->content.string->text, "abc");
	}

	TEST_SECTION("parse_arguments 2 個");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "abc def \n");
		t_token	tok;

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_arguments(&buf, &tok);
		CHECK(node);
		CHECK_EQ(node->type, ASTNODE_ARGUMENTS);
		t_parse_ast *str_node = node->content.arguments->string_node;
		CHECK(str_node);
		CHECK_EQ(str_node->type, ASTNODE_STRING);
		CHECK_EQ_STR(str_node->content.string->text, "abc");

		t_parse_ast *rest_node = node->content.arguments->rest_node;

		CHECK_EQ(rest_node->type, ASTNODE_ARGUMENTS);
		str_node = rest_node->content.arguments->string_node;
		CHECK(str_node);
		CHECK_EQ(str_node->type, ASTNODE_STRING);
		CHECK_EQ_STR(str_node->content.string->text, "def");
	}

	TEST_SECTION("parse_arguments リダイレクト");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "< abc \n");
		t_token	tok;

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_arguments(&buf, &tok);
		CHECK(node);
		CHECK_EQ(node->type, ASTNODE_ARGUMENTS);

		t_parse_ast *red_node = node->content.arguments->redirection_node;
		CHECK(red_node);
		CHECK_EQ(red_node->type, ASTNODE_REDIRECTION);
		CHECK_EQ(red_node->content.redirection->type, TOKTYPE_INPUT_REDIRECTION);
		CHECK_EQ_STR(red_node->content.redirection->string_node
					->content.string->text, "abc");
	}

	TEST_SECTION("parse_arguments ファイル + リダイレクト");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "file < abc \n");
		t_token	tok;

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_arguments(&buf, &tok);
		check_args(node);
	}

	TEST_SECTION("parse_command ファイル + リダイレクト");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "file < abc \n");
		t_token	tok;

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_command(&buf, &tok);
		CHECK_EQ(node->type, ASTNODE_COMMAND);
		check_args(node->content.command->arguments_node);
	}

	TEST_SECTION("parse_piped_commands パイプなし");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "abc \n");
		t_token	tok;

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_piped_commands(&buf, &tok);
		CHECK_EQ(node->type, ASTNODE_PIPED_COMMANDS);

		t_parse_ast *cmd = node->content.piped_commands->command_node;
		CHECK(cmd);
		CHECK_EQ(cmd->type, ASTNODE_COMMAND);
		t_parse_ast *str = cmd->content.command->arguments_node
			->content.arguments->string_node;
		CHECK(str);
		CHECK_EQ_STR(str->content.string->text, "abc");
		CHECK(!node->content.piped_commands->next);
	}

	TEST_SECTION("parse_piped_commands パイプあり");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "abc | file < abc \n");
		t_token	tok;

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_piped_commands(&buf, &tok);
		check_piped_commands(node);
	}

	TEST_SECTION("parse_delimiter");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "; \n");
		t_token	tok;

		lex_get_token(&buf, &tok);
		t_parse_ast *node = parse_delimiter(&buf, &tok);
		check_delimiter(node);
	}

	TEST_SECTION("parse_sequential_commands 単純なケース");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, " abc ; xyz \n");
		t_token	tok;

		lex_get_token(&buf, &tok);
		t_parse_ast *node = parse_sequential_commands(&buf, &tok);
		check_single_argument(
			node->content.sequential_commands
			->pipcmd_node->content.piped_commands
			->command_node->content.command
			->arguments_node,
			"abc");
		check_delimiter(
			node->content.sequential_commands
			->delimiter_node);
		check_single_argument(
			node->content.sequential_commands
			->rest_node->content.sequential_commands
			->pipcmd_node->content.piped_commands
			->command_node->content.command
			->arguments_node,
			"xyz");
	}

	TEST_SECTION("parse_sequential_commands パイプあり");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, " abc | file < abc ; xyz \n");
		t_token	tok;

		lex_get_token(&buf, &tok);
		t_parse_ast *node = parse_sequential_commands(&buf, &tok);

		check_piped_seqence(node);
	}

	TEST_SECTION("parse_command_line パイプなし");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "abc\n");
		t_token	tok;

		lex_get_token(&buf, &tok);
		t_parse_ast *node = parse_command_line(&buf, &tok);
		check_single_argument(
			node->content.command_line->seqcmd_node
			->content.sequential_commands
			->pipcmd_node->content.piped_commands
			->command_node->content.command
			->arguments_node,
			"abc");
	}

	TEST_SECTION("parse_command_line");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, " abc | file < abc ; xyz \n");
		t_token	tok;

		lex_get_token(&buf, &tok);
		t_parse_ast *node = parse_command_line(&buf, &tok);

		check_piped_seqence(node->content.command_line->seqcmd_node);
		CHECK_EQ(node->content.command_line->delimiter_node, NULL);
	}
}

int main()
{
	test_lexer();
	test_parser();
	int fail_count = print_result();
	return (fail_count);
}
