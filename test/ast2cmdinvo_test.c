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
		if (actual_cmdinvo->input_redirections || expected_invo->input_redirections)
		{
			t_list	*current_actual = actual_cmdinvo->input_redirections;
			t_list	*current_expected = expected_invo->input_redirections;
			while (current_actual || current_expected)
			{
				t_cmd_redirection	*red_actual = (t_cmd_redirection *)current_actual->content;
				t_cmd_redirection	*red_expected = (t_cmd_redirection *)current_expected->content;

				CHECK(red_actual->filepath && red_expected->filepath);
				CHECK_EQ_STR(red_actual->filepath, red_expected->filepath);

				current_actual = current_actual->next;
				current_expected = current_expected->next;
			}
		}
		if (actual_cmdinvo->output_redirections || expected_invo->output_redirections)
		{
			t_list	*current_actual = actual_cmdinvo->output_redirections;
			t_list	*current_expected = expected_invo->output_redirections;
			while (current_actual || current_expected)
			{
				t_cmd_redirection	*red_actual = (t_cmd_redirection *)current_actual->content;
				t_cmd_redirection	*red_expected = (t_cmd_redirection *)current_expected->content;

				CHECK(red_actual->filepath && red_expected->filepath);
				CHECK_EQ_STR(red_actual->filepath, red_expected->filepath);
				CHECK(red_actual->is_append == red_expected->is_append);

				current_actual = current_actual->next;
				current_expected = current_expected->next;
			}
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
		t_command_invocation *expected = cmd_init_cmdinvo((const char **)ft_split("abc", ' '));
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
		t_command_invocation *expected = cmd_init_cmdinvo((const char **)ft_split("abc def", ' '));
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
		t_command_invocation *expected = cmd_init_cmdinvo((const char **)ft_split("file", ' '));
		cmd_add_inredirect(expected, ft_strdup("abc"));
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
		t_command_invocation *expected = cmd_init_cmdinvo((const char **)ft_split("file", ' '));
		cmd_add_outredirect(expected, ft_strdup("abc"), false);
		CHECK(actual);
		check_cmdinvo(actual, expected);

		cmd_free_cmdinvo(actual);
		cmd_free_cmdinvo(expected);
	}

	TEST_SECTION("cmd_ast_cmd2cmdinvo 入出力リダイレクト付き");
	{
		/* 準備 */
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "file < input > output \n");
		t_token	tok;

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_command(&buf, &tok);
        CHECK_EQ(node->type, ASTNODE_COMMAND);

		/* テスト */
		t_command_invocation *actual = cmd_ast_cmd2cmdinvo(node->content.command);
		t_command_invocation *expected = cmd_init_cmdinvo((const char **)ft_split("file", ' '));
		cmd_add_inredirect(expected, ft_strdup("input"));
		cmd_add_outredirect(expected, ft_strdup("output"), false);
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
		t_command_invocation *expected = cmd_init_cmdinvo((const char **)ft_split("file", ' '));
		cmd_add_outredirect(expected, ft_strdup("abc"), true);
		CHECK(actual);
		check_cmdinvo(actual, expected);

		cmd_free_cmdinvo(actual);
		cmd_free_cmdinvo(expected);
	}

	TEST_SECTION("cmd_ast_cmd2cmdinvo 複数入力リダイレクト");
	{
		/* 準備 */
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "file < a < b < c < d \n");
		t_token	tok;

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_command(&buf, &tok);
        CHECK_EQ(node->type, ASTNODE_COMMAND);

		/* テスト */
		t_command_invocation *actual = cmd_ast_cmd2cmdinvo(node->content.command);
		t_command_invocation *expected = cmd_init_cmdinvo((const char **)ft_split("file", ' '));
		cmd_add_inredirect(expected, ft_strdup("a"));
		cmd_add_inredirect(expected, ft_strdup("b"));
		cmd_add_inredirect(expected, ft_strdup("c"));
		cmd_add_inredirect(expected, ft_strdup("d"));
		CHECK(actual);
		check_cmdinvo(actual, expected);

		cmd_free_cmdinvo(actual);
		cmd_free_cmdinvo(expected);
	}

	TEST_SECTION("cmd_ast_cmd2cmdinvo 複数書き込みリダイレクト");
	{
		/* 準備 */
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "file > a > b > c > d \n");
		t_token	tok;

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_command(&buf, &tok);
        CHECK_EQ(node->type, ASTNODE_COMMAND);

		/* テスト */
		t_command_invocation *actual = cmd_ast_cmd2cmdinvo(node->content.command);
		t_command_invocation *expected = cmd_init_cmdinvo((const char **)ft_split("file", ' '));
		cmd_add_outredirect(expected, ft_strdup("a"), false);
		cmd_add_outredirect(expected, ft_strdup("b"), false);
		cmd_add_outredirect(expected, ft_strdup("c"), false);
		cmd_add_outredirect(expected, ft_strdup("d"), false);
		CHECK(actual);
		check_cmdinvo(actual, expected);

		cmd_free_cmdinvo(actual);
		cmd_free_cmdinvo(expected);
	}

	TEST_SECTION("cmd_ast_cmd2cmdinvo 複数追記リダイレクト");
	{
		/* 準備 */
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "file >> a >> b >> c >> d \n");
		t_token	tok;

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_command(&buf, &tok);
        CHECK_EQ(node->type, ASTNODE_COMMAND);

		/* テスト */
		t_command_invocation *actual = cmd_ast_cmd2cmdinvo(node->content.command);
		t_command_invocation *expected = cmd_init_cmdinvo((const char **)ft_split("file", ' '));
		cmd_add_outredirect(expected, ft_strdup("a"), true);
		cmd_add_outredirect(expected, ft_strdup("b"), true);
		cmd_add_outredirect(expected, ft_strdup("c"), true);
		cmd_add_outredirect(expected, ft_strdup("d"), true);
		CHECK(actual);
		check_cmdinvo(actual, expected);

		cmd_free_cmdinvo(actual);
		cmd_free_cmdinvo(expected);
	}

	TEST_SECTION("cmd_ast_cmd2cmdinvo 複数入力リダイレクト");
	{
		/* 準備 */
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "file < a < b > c > d \n");
		t_token	tok;

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_command(&buf, &tok);
        CHECK_EQ(node->type, ASTNODE_COMMAND);

		/* テスト */
		t_command_invocation *actual = cmd_ast_cmd2cmdinvo(node->content.command);
		t_command_invocation *expected = cmd_init_cmdinvo((const char **)ft_split("file", ' '));
		cmd_add_inredirect(expected, ft_strdup("a"));
		cmd_add_inredirect(expected, ft_strdup("b"));
		cmd_add_outredirect(expected, ft_strdup("c"), false);
		cmd_add_outredirect(expected, ft_strdup("d"), false);
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
		t_command_invocation *expected = cmd_init_cmdinvo((const char **)ft_split("abc", ' '));
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
		t_command_invocation *expected = cmd_init_cmdinvo((const char **)ft_split("abc def", ' '));
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
		t_command_invocation *expected_first = cmd_init_cmdinvo((const char **)ft_split("abc", ' '));
		t_command_invocation *expected_second = cmd_init_cmdinvo((const char **)ft_split("file", ' '));
		cmd_add_inredirect(expected_second, ft_strdup("abc"));
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
		t_command_invocation *expected_first = cmd_init_cmdinvo((const char **)ft_split("abc", ' '));
		t_command_invocation *expected_second = cmd_init_cmdinvo((const char **)ft_split("file", ' '));
		cmd_add_outredirect(expected_second, ft_strdup("abc"), 0);
		expected_first->piped_command = expected_second;

		CHECK(actual);
		check_cmdinvo(actual, expected_first);

		cmd_free_cmdinvo(actual);
		cmd_free_cmdinvo(expected_first);
	}

	parse_free_all_ast();
	int fail_count = print_result();
	return (fail_count);
}
