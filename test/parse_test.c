#include "test.h"
#include "../parse.h"

void	init_buf_with_string(t_parse_buffer *buf, const char* str)
{
	buf->cur_pos = 0;
	strcpy(buf->buffer, str);
	buf->size = strlen(str);
	buf->lex_stat = LEXSTAT_NORMAL;
	buf->getc = NULL;
	buf->ungetc = NULL;
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
		lex_init_token(&tok);
		lex_read_word(&buf, &tok);
		CHECK_EQ(tok.length, 3);
		CHECK(!strncmp(tok.text, "cat", 3));
		free(tok.text);
	}

	TEST_SECTION("lex_get_token デスクリプタ付きのリダイレクト");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "10< 123> 456>> ");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_INPUT_REDIRECTION);
		CHECK_EQ(tok.length, 10);

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_SPACE);

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_OUTPUT_REDIRECTION);
		CHECK_EQ(tok.length, 123);

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_SPACE);

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_OUTPUT_APPENDING);
		CHECK_EQ(tok.length, 456);
		free(tok.text);
	}

	TEST_SECTION("lex_get_token クォートなしの場合");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "cat $ABC |wc> file.txt\n");
		t_token	tok;
		lex_init_token(&tok);

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
		CHECK_EQ(tok.length, 1); /* デフォルトで標準出力 (1) */
		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_SPACE);

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_EXPANDABLE);
		CHECK_EQ(tok.length, 8);
		CHECK(!strncmp(tok.text, "file.txt", 8));

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_NEWLINE);
		free(tok.text);
	}

	TEST_SECTION("lex_get_token クォートなしエスケープありの場合");
	{
		t_parse_buffer	buf;
		// \$ABC c\ at \"xyz
		init_buf_with_string(&buf, "\\$ABC c\\ at \\\"xyz \n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_NON_EXPANDABLE);
		CHECK_EQ(tok.length, 1);
		CHECK(!strncmp(tok.text, "$", 1));

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_EXPANDABLE);
		CHECK_EQ(tok.length, 3);
		CHECK(!strncmp(tok.text, "ABC", 3));

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_SPACE);

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_EXPANDABLE);
		CHECK_EQ(tok.length, 1);
		CHECK(!strncmp(tok.text, "c", 1));

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_NON_EXPANDABLE);
		CHECK_EQ(tok.length, 1);
		CHECK(!strncmp(tok.text, " ", 1));

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_EXPANDABLE);
		CHECK_EQ(tok.length, 2);
		CHECK(!strncmp(tok.text, "at", 2));

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_SPACE);

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_NON_EXPANDABLE);
		CHECK_EQ(tok.length, 1);
		CHECK(!strncmp(tok.text, "\"", 1));

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_EXPANDABLE);
		CHECK_EQ(tok.length, 3);
		CHECK(!strncmp(tok.text, "xyz", 3));

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_SPACE);

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_NEWLINE);
		free(tok.text);
	}

	TEST_SECTION("lex_get_token クォートありの場合");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "cat\"$ABC\"'wc'");
		t_token	tok;
		lex_init_token(&tok);

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
		free(tok.text);
	}

	TEST_SECTION("lex_get_token クォートありエスケープあり");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "\"\\$ABC\" '\\abc'");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_NON_EXPANDABLE);
		CHECK_EQ(tok.length, 1);
		CHECK(!strncmp(tok.text, "$", 1));

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_EXPANDABLE_QUOTED);
		CHECK_EQ(tok.length, 3);
		CHECK(!strncmp(tok.text, "ABC", 3));

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_SPACE);

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_NON_EXPANDABLE);
		CHECK_EQ(tok.length, 4);
		CHECK(!strncmp(tok.text, "\\abc", 4));
		free(tok.text);
	}

	TEST_SECTION("lex_get_token クォートありエスケープ2つあり");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "\"\\\\$ABC\" '\\\\abc'");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_NON_EXPANDABLE);
		CHECK_EQ(tok.length, 1);
		CHECK(!strncmp(tok.text, "\\", 1));

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_EXPANDABLE_QUOTED);
		CHECK_EQ(tok.length, 4);
		CHECK(!strncmp(tok.text, "$ABC", 4));

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_SPACE);

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_NON_EXPANDABLE);
		CHECK_EQ(tok.length, 5);
		CHECK(!strncmp(tok.text, "\\\\abc", 5));
		free(tok.text);
	}

	TEST_SECTION("lex_get_token 数字だけのトークン");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "111 22 abcd ");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_EXPANDABLE);
		CHECK_EQ(tok.length, 3);
		CHECK(!strncmp(tok.text, "111", 3));

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_SPACE);

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_EXPANDABLE);
		CHECK_EQ(tok.length, 2);
		CHECK(!strncmp(tok.text, "22", 3));

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_SPACE);

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_EXPANDABLE);
		CHECK_EQ(tok.length, 4);
		CHECK(!strncmp(tok.text, "abcd", 4));
		free(tok.text);
	}

	TEST_SECTION("lex_get_token #88");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "chmod 000 dir ");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_EXPANDABLE);
		CHECK_EQ(tok.length, 5);
		CHECK(!strncmp(tok.text, "chmod", 5));

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_SPACE);

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_EXPANDABLE);
		CHECK_EQ(tok.length, 3);
		CHECK(!strncmp(tok.text, "000", 3));

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_SPACE);

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_EXPANDABLE);
		CHECK_EQ(tok.length, 3);
		CHECK(!strncmp(tok.text, "dir", 3));
		free(tok.text);
	}

	TEST_SECTION("lex_get_token のこり");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "cat < - ; ");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_EXPANDABLE);
		CHECK_EQ(tok.length, 3);
		CHECK(!strncmp(tok.text, "cat", 3));

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_SPACE);

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_INPUT_REDIRECTION);
		CHECK_EQ(tok.length, 0); /* デフォルトで標準入力 (0) */

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
		free(tok.text);
	}

	TEST_SECTION("lex_get_token EOF");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "");
		t_token	tok;
		lex_init_token(&tok);
		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_EOF);
		free(tok.text);
	}

	TEST_SECTION("lex_get_token ヒアドキュメント");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "<< ");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_HEREDOCUMENT);
		CHECK_EQ(tok.length, 0);

		free(tok.text);
	}

	TEST_SECTION("lex_get_token ヒアドキュメント fd あり");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "123<< ");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_HEREDOCUMENT);
		CHECK_EQ(tok.length, 123);

		free(tok.text);
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

void check_output_redirection(t_parse_ast	*red_node, const char *name)
{
	CHECK(red_node);
	CHECK_EQ(red_node->type, ASTNODE_REDIRECTION);
	CHECK_EQ(red_node->content.redirection->type, TOKTYPE_OUTPUT_REDIRECTION);
	check_string(red_node->content.redirection->string_node, name);
}

void check_heredocument(t_parse_ast	*red_node, const char *name)
{
	CHECK(red_node);
	CHECK_EQ(red_node->type, ASTNODE_REDIRECTION);
	CHECK_EQ(red_node->content.redirection->type, TOKTYPE_HEREDOCUMENT);
	check_string(red_node->content.redirection->string_node, name);
}

void check_redirection(t_parse_ast	*red_node, const char *name)
{
	CHECK(red_node);
	CHECK_EQ(red_node->type, ASTNODE_REDIRECTION);
	CHECK_EQ(red_node->content.redirection->type, TOKTYPE_INPUT_REDIRECTION);
	check_string(red_node->content.redirection->string_node, name);
}

void check_args(t_parse_ast	*node)
{
	CHECK(node);
	CHECK_EQ(node->type, ASTNODE_ARGUMENTS);

	check_single_argument(node, "file");

	node = node->content.arguments->rest_node;
	t_parse_ast *red_node = node->content.arguments->redirection_node;
	check_redirection(red_node, "abc");
	CHECK(!node->content.arguments->rest_node);
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

void check_pip_heredocument(
	t_parse_ast *node, const char *cmd, const char *eod, int fd)
{
	CHECK_EQ(node->type, ASTNODE_PIPED_COMMANDS);
	node = node->content.piped_commands
		->command_node->content.command
		->arguments_node;
	CHECK(node);
	check_single_argument(node, cmd);

	t_parse_ast *red_node = node->content.arguments->rest_node
		->content.arguments->redirection_node;
	CHECK(red_node);
	CHECK_EQ(red_node->type, ASTNODE_REDIRECTION);
	CHECK_EQ(red_node->content.redirection->type, TOKTYPE_HEREDOCUMENT);
	CHECK_EQ(red_node->content.redirection->fd, fd);

	check_heredocument(red_node, eod);
}

void test_parser(void)
{
	TEST_CHAPTER("パーサ");

	TEST_SECTION("parse_string 単純な文字列");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "file\n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_string(&buf, &tok);
		CHECK(node);
		CHECK_EQ(node->type, ASTNODE_STRING);
		CHECK_EQ(node->content.string->type, TOKTYPE_EXPANDABLE);
		CHECK_EQ_STR(node->content.string->text, "file");
		CHECK_EQ(node->content.string->next, NULL);
		free(tok.text);
	}

	TEST_SECTION("parse_string シングルクォート");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "'file'\n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_string(&buf, &tok);
		CHECK(node);
		CHECK_EQ(node->type, ASTNODE_STRING);
		CHECK_EQ(node->content.string->type, TOKTYPE_NON_EXPANDABLE);
		CHECK_EQ_STR(node->content.string->text, "file");
		CHECK_EQ(node->content.string->next, NULL);
		free(tok.text);
	}

	TEST_SECTION("parse_string ダブルクォート");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "\"file\"\n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_string(&buf, &tok);
		CHECK(node);
		CHECK_EQ(node->type, ASTNODE_STRING);
		CHECK_EQ(node->content.string->type, TOKTYPE_EXPANDABLE_QUOTED);
		CHECK_EQ_STR(node->content.string->text, "file");
		CHECK_EQ(node->content.string->next, NULL);
		free(tok.text);
	}

	TEST_SECTION("parse_string ミックス");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "abc\"def\"'ghi'\n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_string(&buf, &tok);
		CHECK(node);
		CHECK_EQ(node->type, ASTNODE_STRING);
		CHECK_EQ(node->content.string->type, TOKTYPE_EXPANDABLE);
		CHECK_EQ_STR(node->content.string->text, "abc");

		node = node->content.string->next;
		CHECK(node);
		CHECK_EQ(node->type, ASTNODE_STRING);
		CHECK_EQ(node->content.string->type, TOKTYPE_EXPANDABLE_QUOTED);
		CHECK_EQ_STR(node->content.string->text, "def");

		node = node->content.string->next;
		CHECK(node);
		CHECK_EQ(node->type, ASTNODE_STRING);
		CHECK_EQ(node->content.string->type, TOKTYPE_NON_EXPANDABLE);
		CHECK_EQ_STR(node->content.string->text, "ghi");
		free(tok.text);
	}

	TEST_SECTION("parse_redirection　<");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "< file\n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_redirection(&buf, &tok);
		CHECK(node);
		CHECK_EQ(node->type, ASTNODE_REDIRECTION);
		CHECK_EQ(node->content.redirection->fd, 0);
		CHECK_EQ(node->content.redirection->type, TOKTYPE_INPUT_REDIRECTION);
		CHECK_EQ_STR(node->content.redirection->string_node
					->content.string->text, "file");
		free(tok.text);
	}

	TEST_SECTION("parse_redirection デスクリプタ付き <");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "123< file\n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_redirection(&buf, &tok);
		CHECK(node);
		CHECK_EQ(node->type, ASTNODE_REDIRECTION);
		CHECK_EQ(node->content.redirection->fd, 123);
		CHECK_EQ(node->content.redirection->type, TOKTYPE_INPUT_REDIRECTION);
		CHECK_EQ_STR(node->content.redirection->string_node
					->content.string->text, "file");
		free(tok.text);
	}

	TEST_SECTION("parse_redirection　>");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "> file\n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_redirection(&buf, &tok);
		CHECK(node);
		CHECK_EQ(node->type, ASTNODE_REDIRECTION);
		CHECK_EQ(node->content.redirection->fd, 1);
		CHECK_EQ(node->content.redirection->type, TOKTYPE_OUTPUT_REDIRECTION);
		CHECK_EQ_STR(node->content.redirection->string_node
					 ->content.string->text, "file");
		free(tok.text);
	}

	TEST_SECTION("parse_redirection　デスクリプタ付き >");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "123> file\n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_redirection(&buf, &tok);
		CHECK(node);
		CHECK_EQ(node->type, ASTNODE_REDIRECTION);
		CHECK_EQ(node->content.redirection->fd, 123);
		CHECK_EQ(node->content.redirection->type, TOKTYPE_OUTPUT_REDIRECTION);
		CHECK_EQ_STR(node->content.redirection->string_node
					 ->content.string->text, "file");
		free(tok.text);
	}

	TEST_SECTION("parse_redirection　fdの最大最大値付き >");
	{
		// bashではfdの値がINTMAX以下の正の数の場合ファイルディスクリプタとして認識される
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "2147483647> file\n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_redirection(&buf, &tok);
		CHECK(node);
		CHECK_EQ(node->type, ASTNODE_REDIRECTION);
		CHECK_EQ(node->content.redirection->fd, 2147483647);
		CHECK_EQ(node->content.redirection->type, TOKTYPE_OUTPUT_REDIRECTION);
		CHECK_EQ_STR(node->content.redirection->string_node
					 ->content.string->text, "file");
		free(tok.text);
	}

	TEST_SECTION("parse_redirection　fdがINTMAX超過 >");
	{
		// bashではfdの値がINTMAX超過の場合ファイルディスクリプタではなくコマンド引数として認識される
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "2147483648> file\n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);
		t_parse_ast *string_node = parse_string(&buf, &tok);
		CHECK(string_node);
		CHECK_EQ(string_node->type, ASTNODE_STRING);
		CHECK_EQ(string_node->content.string->type, TOKTYPE_EXPANDABLE);
		CHECK_EQ_STR(string_node->content.string->text, "2147483648");
		t_parse_ast *node = parse_redirection(&buf, &tok);
		CHECK(node);
		CHECK_EQ(node->type, ASTNODE_REDIRECTION);
		CHECK_EQ(node->content.redirection->fd, 1);
		CHECK_EQ(node->content.redirection->type, TOKTYPE_OUTPUT_REDIRECTION);
		CHECK_EQ_STR(node->content.redirection->string_node
					 ->content.string->text, "file");
		free(tok.text);
	}

	TEST_SECTION("parse_redirection　>>");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, ">> file\n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_redirection(&buf, &tok);
		CHECK(node);
		CHECK_EQ(node->type, ASTNODE_REDIRECTION);
		CHECK_EQ(node->content.redirection->fd, 1);
		CHECK_EQ(node->content.redirection->type, TOKTYPE_OUTPUT_APPENDING);
		CHECK_EQ_STR(node->content.redirection->string_node
					->content.string->text, "file");
		free(tok.text);
	}

	TEST_SECTION("parse_redirection デスクリプタ付き >>");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "456>> file\n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_redirection(&buf, &tok);
		CHECK(node);
		CHECK_EQ(node->type, ASTNODE_REDIRECTION);
		CHECK(!node->heredocs);
		CHECK_EQ(node->content.redirection->fd, 456);
		CHECK_EQ(node->content.redirection->type, TOKTYPE_OUTPUT_APPENDING);
		CHECK_EQ_STR(node->content.redirection->string_node
					->content.string->text, "file");
		free(tok.text);
	}

	TEST_SECTION("parse_redirection　<<");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "<< EOD\n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_redirection(&buf, &tok);
		CHECK(node);
		CHECK_EQ(node->type, ASTNODE_REDIRECTION);

		CHECK(node->heredocs);
		CHECK(!node->heredocs->next);
		check_string(node->heredocs->redirection->string_node, "EOD");

		CHECK_EQ(node->content.redirection->fd, 0);
		CHECK_EQ(node->content.redirection->type, TOKTYPE_HEREDOCUMENT);
		CHECK_EQ_STR(node->content.redirection->string_node
					->content.string->text, "EOD");
		free(tok.text);
		parse_free_heredocs(node->heredocs);
	}

	TEST_SECTION("parse_redirection デスクリプタ付き <<");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "456<< EOD\n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_redirection(&buf, &tok);
		CHECK(node);
		CHECK_EQ(node->type, ASTNODE_REDIRECTION);

		CHECK(node->heredocs);
		CHECK(!node->heredocs->next);
		check_string(node->heredocs->redirection->string_node, "EOD");

		CHECK_EQ(node->content.redirection->fd, 456);
		CHECK_EQ(node->content.redirection->type, TOKTYPE_HEREDOCUMENT);
		CHECK_EQ_STR(node->content.redirection->string_node
					->content.string->text, "EOD");
		free(tok.text);
		parse_free_heredocs(node->heredocs);
	}

	TEST_SECTION("parse_arguments 1 個");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "abc \n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_arguments(&buf, &tok);
		CHECK(node);
		CHECK_EQ(node->type, ASTNODE_ARGUMENTS);
		t_parse_ast *str_node = node->content.arguments->string_node;
		CHECK(str_node);
		CHECK_EQ(str_node->type, ASTNODE_STRING);
		CHECK_EQ_STR(str_node->content.string->text, "abc");
		free(tok.text);
	}

	TEST_SECTION("parse_arguments 1 個直後に改行");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "abc\n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_arguments(&buf, &tok);
		CHECK(node);
		CHECK_EQ(node->type, ASTNODE_ARGUMENTS);
		t_parse_ast *str_node = node->content.arguments->string_node;
		CHECK(str_node);
		CHECK_EQ(str_node->type, ASTNODE_STRING);
		CHECK_EQ_STR(str_node->content.string->text, "abc");
		free(tok.text);
	}

	TEST_SECTION("parse_arguments 2 個");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "abc def \n");
		t_token	tok;
		lex_init_token(&tok);

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
		free(tok.text);
	}

	TEST_SECTION("parse_arguments リダイレクト");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "< abc \n");
		t_token	tok;
		lex_init_token(&tok);

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
		free(tok.text);
	}

	TEST_SECTION("parse_arguments ファイル + リダイレクト");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "file < abc \n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_arguments(&buf, &tok);
		check_args(node);
		free(tok.text);
	}

	TEST_SECTION("parse_arguments ファイル + リダイレクト 2 個");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "file < abc < def\n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_arguments(&buf, &tok);
		CHECK(node);
		CHECK_EQ(node->type, ASTNODE_ARGUMENTS);
		check_single_argument(node, "file");
		node = node->content.arguments->rest_node;
		t_parse_ast *red_node = node->content.arguments->redirection_node;
		check_redirection(red_node, "abc");

		node = node->content.arguments->rest_node;
		red_node = node->content.arguments->redirection_node;
		check_redirection(red_node, "def");
		free(tok.text);
	}

	TEST_SECTION("parse_arguments ファイル + 出力リダイレクト 2 個");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "file > abc > def\n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_arguments(&buf, &tok);
		CHECK(node);
		CHECK_EQ(node->type, ASTNODE_ARGUMENTS);
		check_single_argument(node, "file");
		node = node->content.arguments->rest_node;
		t_parse_ast *red_node = node->content.arguments->redirection_node;
		check_output_redirection(red_node, "abc");

		node = node->content.arguments->rest_node;
		red_node = node->content.arguments->redirection_node;
		check_output_redirection(red_node, "def");
		free(tok.text);
	}

	TEST_SECTION("parse_command ファイル + リダイレクト");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "file < abc \n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_command(&buf, &tok);
		CHECK_EQ(node->type, ASTNODE_COMMAND);
		check_args(node->content.command->arguments_node);
		free(tok.text);
	}

	TEST_SECTION("parse_piped_commands パイプなし");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "abc \n");
		t_token	tok;
		lex_init_token(&tok);

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
		free(tok.text);
	}

	TEST_SECTION("parse_piped_commands パイプあり");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "abc | file < abc \n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_piped_commands(&buf, &tok);
		check_piped_commands(node);
		free(tok.text);
	}

	TEST_SECTION("parse_delimiter");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "; \n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);
		t_parse_ast *node = parse_delimiter(&buf, &tok);
		check_delimiter(node);
		free(tok.text);
	}

	TEST_SECTION("parse_sequential_commands 単純なケース");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, " abc ; xyz \n");
		t_token	tok;
		lex_init_token(&tok);

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
		free(tok.text);
	}

	TEST_SECTION("parse_sequential_commands セミコロンで終わる");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, " abc ; xyz ;\n");
		t_token	tok;
		lex_init_token(&tok);

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
		check_delimiter(
			node->content.sequential_commands
			->rest_node->content.sequential_commands
			->delimiter_node);
		free(tok.text);
	}

	TEST_SECTION("parse_sequential_commands パイプあり");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, " abc | file < abc ; xyz \n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);
		t_parse_ast *node = parse_sequential_commands(&buf, &tok);

		check_piped_seqence(node);
		free(tok.text);
	}

	TEST_SECTION("parse_command_line パイプなし");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "abc\n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);
		t_parse_ast *node = parse_command_line(&buf, &tok);
		check_single_argument(
			node->content.command_line->seqcmd_node
			->content.sequential_commands
			->pipcmd_node->content.piped_commands
			->command_node->content.command
			->arguments_node,
			"abc");
		free(tok.text);
	}

	TEST_SECTION("parse_command_line");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, " abc | file < abc ; xyz \n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);
		t_parse_ast *node = parse_command_line(&buf, &tok);

		check_piped_seqence(node->content.command_line->seqcmd_node);
		free(tok.text);
	}

	TEST_SECTION("parse_command_line バックスラッシュで終わる");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "abc\\\n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);
		t_parse_ast *node = parse_command_line(&buf, &tok);

		check_single_argument(
			node->content.command_line->seqcmd_node
			->content.sequential_commands
			->pipcmd_node->content.piped_commands
			->command_node->content.command
			->arguments_node,
			"abc");
		free(tok.text);
	}

	TEST_SECTION("parse_command_line 閉じてないダブルクォート");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "abc\"\n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);
		t_parse_ast *node = parse_command_line(&buf, &tok);
		CHECK(!node);
		free(tok.text);
	}

	TEST_SECTION("parse_command_line 閉じてないシングルクォート");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "abc'\n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);
		t_parse_ast *node = parse_command_line(&buf, &tok);
		CHECK(!node);
		free(tok.text);
	}

	TEST_SECTION("parse_command_line セミコロンで終わる");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, " abc ; xyz ;\n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);
		t_parse_ast *node = parse_command_line(&buf, &tok);
		CHECK_EQ(node->type, ASTNODE_COMMAND_LINE);
		node = node->content.command_line->seqcmd_node;
		CHECK(node);

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
		check_delimiter(
			node->content.sequential_commands
			->rest_node->content.sequential_commands
			->delimiter_node);
		free(tok.text);
	}

	TEST_SECTION("parse_command_line セミコロンだけのときはエラー");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, " ;\n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);
		t_parse_ast *node = parse_command_line(&buf, &tok);
		CHECK(!node);
		free(tok.text);
	}

	TEST_SECTION("parse_command_line > のあとにファイル名が続かないときはエラー");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "aho >\n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);
		t_parse_ast *node = parse_command_line(&buf, &tok);
		CHECK(!node);
		free(tok.text);
	}

	TEST_SECTION("parse_command_line > のあとに ; はエラー");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "aho >; boke\n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);
		t_parse_ast *node = parse_command_line(&buf, &tok);
		CHECK(!node);
		free(tok.text);
	}

	TEST_SECTION("parse_command_line | のあとにコマンドが続かないときはエラー");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "aho |\n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);
		t_parse_ast *node = parse_command_line(&buf, &tok);
		CHECK(!node);
		free(tok.text);
	}

	TEST_SECTION("parse_command_line | のあとに ; はエラー");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "aho |; boke\n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);
		t_parse_ast *node = parse_command_line(&buf, &tok);
		CHECK(!node);
		free(tok.text);
	}

	TEST_SECTION("parse_command_line ヒアドキュメント");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "abc << def\n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);
		t_parse_ast *cmdline_node = parse_command_line(&buf, &tok);
		CHECK(cmdline_node);
		CHECK(cmdline_node->heredocs);

		CHECK(!cmdline_node->heredocs->next);
		check_string(cmdline_node->heredocs->redirection->string_node, "def");

		t_parse_ast *node = cmdline_node->content.command_line
			->seqcmd_node->content.sequential_commands
			->pipcmd_node;
		CHECK(node);
		check_pip_heredocument(node, "abc", "def", 0);

		free(tok.text);
		parse_free_heredocs(node->heredocs);
	}

	TEST_SECTION("parse_command_line ヒアドキュメント 2 個");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "abc << def ; g ; hij 123<< klm\n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);
		t_parse_ast *cmdline_node = parse_command_line(&buf, &tok);
		CHECK(cmdline_node);

		CHECK(cmdline_node->heredocs);
		check_string(cmdline_node->heredocs->redirection->string_node, "def");
		CHECK(cmdline_node->heredocs->next);
		check_string(cmdline_node->heredocs
					 ->next->redirection->string_node, "klm");
		CHECK(!cmdline_node->heredocs->next->next);

		t_parse_ast *node = cmdline_node->content.command_line
			->seqcmd_node;
		CHECK(node);

		t_parse_ast *here_node = node->content.sequential_commands
			->pipcmd_node;
		CHECK(here_node);
		check_pip_heredocument(here_node, "abc", "def", 0);

		node = node->content.sequential_commands->rest_node;
		CHECK_EQ(node->type, ASTNODE_SEQ_COMMANDS);
		check_single_argument(
			node->content.sequential_commands->pipcmd_node
			->content.piped_commands->command_node
			->content.command->arguments_node, "g");

		here_node = node->content.sequential_commands->rest_node
			->content.sequential_commands->pipcmd_node;
		CHECK(here_node);
		check_pip_heredocument(here_node, "hij", "klm", 123);

		free(tok.text);
	}

}

int main()
{
	test_lexer();
	test_parser();
	parse_free_all_ast();
	int fail_count = print_result();
	return (fail_count);
}
