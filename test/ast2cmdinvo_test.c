#include "test.h"
#include "../parse.h"
#include "../execution.h"
#include "../minishell.h"

void	init_buf_with_string(t_parse_buffer *buf, const char* str)
{
	buf->cur_pos = 0;
	strcpy(buf->buffer, str);
	buf->size = strlen(str);
}

void check_strarr(const char **actual_strarr, const char **expected_strarr)
{
	int i = 0 ;
	while (actual_strarr[i] && expected_strarr[i])
	{
		CHECK_EQ_STR(actual_strarr[i], expected_strarr[i]);
		i++;
	}
	CHECK(!(actual_strarr[i] || expected_strarr[i]));  // 両方ともNULLだよね?
}

void check_cmdinvo(t_command_invocation *actual_cmdinvo, t_command_invocation *expected_invo)
{
	while (actual_cmdinvo && expected_invo)
	{
		if (actual_cmdinvo->input_file_path || expected_invo->input_file_path)
		{
			CHECK(actual_cmdinvo->input_file_path && expected_invo->input_file_path);
			CHECK_EQ_STR(actual_cmdinvo->input_file_path, expected_invo->input_file_path);
		}
		if (actual_cmdinvo->output_file_path || expected_invo->output_file_path)
		{
			CHECK(actual_cmdinvo->output_file_path && expected_invo->output_file_path);
			CHECK_EQ_STR(actual_cmdinvo->output_file_path, expected_invo->output_file_path);
		}
		check_strarr(actual_cmdinvo->exec_and_args, expected_invo->exec_and_args);

		actual_cmdinvo = actual_cmdinvo->piped_command;
		expected_invo = expected_invo->piped_command;
	}
	CHECK(!(actual_cmdinvo || expected_invo));  // 両方ともNULLだよね?
}

int main()
{
	TEST_CHAPTER("AST to command_invocation");

	TEST_SECTION("cmd_ast_cmd2cmdinvo 文字列1つ");
	{
		/* 準備 */
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "abc \n");
		t_token	tok;

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_command(&buf, &tok);
        CHECK_EQ(node->type, ASTNODE_COMMAND);

		/* テスト */
		t_command_invocation *actual = cmd_ast_cmd2cmdinvo(node->content.command);
		t_command_invocation *expected = cmd_init_cmdinvo(NULL, NULL, (const char **)ft_split("abc", ' '), 0);
		CHECK(actual);
		check_cmdinvo(actual, expected);

		cmd_free_cmdinvo(actual);
		cmd_free_cmdinvo(expected);
	}

	TEST_SECTION("cmd_ast_cmd2cmdinvo arguments 2個");
	{
		/* 準備 */
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "abc def\n");
		t_token	tok;

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_command(&buf, &tok);
        CHECK_EQ(node->type, ASTNODE_COMMAND);

		/* テスト */
		t_command_invocation *actual = cmd_ast_cmd2cmdinvo(node->content.command);
		t_command_invocation *expected = cmd_init_cmdinvo(NULL, NULL, (const char **)ft_split("abc def", ' '), 0);
		CHECK(actual);
		check_cmdinvo(actual, expected);

		cmd_free_cmdinvo(actual);
		cmd_free_cmdinvo(expected);
	}

	TEST_SECTION("cmd_ast_cmd2cmdinvo リダイレクト付き");
	{
		/* 準備 */
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "file < abc \n");
		t_token	tok;

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_command(&buf, &tok);
        CHECK_EQ(node->type, ASTNODE_COMMAND);

		/* テスト */
		t_command_invocation *actual = cmd_ast_cmd2cmdinvo(node->content.command);
		t_command_invocation *expected = cmd_init_cmdinvo(NULL, "abc", (const char **)ft_split("file", ' '), 0);
		CHECK(actual);
		check_cmdinvo(actual, expected);

		cmd_free_cmdinvo(actual);
		cmd_free_cmdinvo(expected);
	}

	TEST_SECTION("cmd_ast_cmd2cmdinvo 出力リダイレクト付き");
	{
		/* 準備 */
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "file > abc \n");
		t_token	tok;

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_command(&buf, &tok);
        CHECK_EQ(node->type, ASTNODE_COMMAND);

		/* テスト */
		t_command_invocation *actual = cmd_ast_cmd2cmdinvo(node->content.command);
		t_command_invocation *expected = cmd_init_cmdinvo("abc", NULL, (const char **)ft_split("file", ' '), CMD_REDIRECT_WRITE);
		CHECK(actual);
		check_cmdinvo(actual, expected);

		cmd_free_cmdinvo(actual);
		cmd_free_cmdinvo(expected);
	}

	TEST_SECTION("cmd_ast_cmd2cmdinvo 追記リダイレクト付き");
	{
		/* 準備 */
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "file >> abc \n");
		t_token	tok;

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_command(&buf, &tok);
        CHECK_EQ(node->type, ASTNODE_COMMAND);

		/* テスト */
		t_command_invocation *actual = cmd_ast_cmd2cmdinvo(node->content.command);
		t_command_invocation *expected = cmd_init_cmdinvo("abc", NULL, (const char **)ft_split("file", ' '), CMD_REDIRECT_APPEND);
		CHECK(actual);
		check_cmdinvo(actual, expected);

		cmd_free_cmdinvo(actual);
		cmd_free_cmdinvo(expected);
	}

	TEST_SECTION("cmd_ast_pipcmds2cmdinvo 文字列1つ");
	{
		/* 準備 */
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "abc \n");
		t_token	tok;

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_piped_commands(&buf, &tok);
        CHECK_EQ(node->type, ASTNODE_PIPED_COMMANDS);

		/* テスト */
		t_command_invocation *actual = cmd_ast_pipcmds2cmdinvo(node->content.piped_commands);
		t_command_invocation *expected = cmd_init_cmdinvo(NULL, NULL, (const char **)ft_split("abc", ' '), 0);
		CHECK(actual);
		check_cmdinvo(actual, expected);

		cmd_free_cmdinvo(actual);
		cmd_free_cmdinvo(expected);
	}

	TEST_SECTION("cmd_ast_pipcmds2cmdinvo arguments 2個");
	{
		/* 準備 */
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "abc def\n");
		t_token	tok;

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_piped_commands(&buf, &tok);
        CHECK_EQ(node->type, ASTNODE_PIPED_COMMANDS);

		/* テスト */
		t_command_invocation *actual = cmd_ast_pipcmds2cmdinvo(node->content.piped_commands);
		t_command_invocation *expected = cmd_init_cmdinvo(NULL, NULL, (const char **)ft_split("abc def", ' '), 0);
		CHECK(actual);
		check_cmdinvo(actual, expected);

		cmd_free_cmdinvo(actual);
		cmd_free_cmdinvo(expected);
	}

	TEST_SECTION("cmd_ast_pipcmds2cmdinvo パイプ & リダイレクション");
	{
		/* 準備 */
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "abc | file < abc \n");
		t_token	tok;

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_piped_commands(&buf, &tok);

		/* テスト */
		t_command_invocation *actual = cmd_ast_pipcmds2cmdinvo(node->content.piped_commands);
		t_command_invocation *expected_first = cmd_init_cmdinvo(NULL, NULL, (const char **)ft_split("abc", ' '), 0);
		t_command_invocation *expected_second = cmd_init_cmdinvo(NULL, "abc", (const char **)ft_split("file", ' '), 0);
		expected_first->piped_command = expected_second;

		CHECK(actual);
		check_cmdinvo(actual, expected_first);

		cmd_free_cmdinvo(actual);
		cmd_free_cmdinvo(expected_first);
	}

	TEST_SECTION("cmd_ast_pipcmds2cmdinvo パイプ & 出力リダイレクション");
	{
		/* 準備 */
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "abc | file > abc \n");
		t_token	tok;

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_piped_commands(&buf, &tok);

		/* テスト */
		t_command_invocation *actual = cmd_ast_pipcmds2cmdinvo(node->content.piped_commands);
		t_command_invocation *expected_first = cmd_init_cmdinvo(NULL, NULL, (const char **)ft_split("abc", ' '), 0);
		t_command_invocation *expected_second = cmd_init_cmdinvo("abc", NULL, (const char **)ft_split("file", ' '), 0);
		expected_first->piped_command = expected_second;

		CHECK(actual);
		check_cmdinvo(actual, expected_first);

		cmd_free_cmdinvo(actual);
		cmd_free_cmdinvo(expected_first);
	}

	int fail_count = print_result();
	return (fail_count);
}
