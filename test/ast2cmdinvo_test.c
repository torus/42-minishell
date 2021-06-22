#include "test.h"
#include "../parse.h"
#include "../execution.h"
#include "../minishell.h"
#include "../env.h"

t_shell	g_shell;

void	init_buf_with_string(t_parse_buffer *buf, const char* str)
{
	buf->cur_pos = 0;
	strcpy(buf->buffer, str);
	buf->size = strlen(str);
	buf->lex_stat = LEXSTAT_NORMAL;
	buf->getc = NULL;
	buf->ungetc = NULL;
}

void check_strarr(const char **actual_strarr, const char **expected_strarr)
{
	int i = 0 ;
	while (actual_strarr[i] && expected_strarr[i])
	{
		printf("%d:\n", i);
		CHECK_EQ_STR(actual_strarr[i], expected_strarr[i]);
		i++;
	}
	printf("%d: |%s| == |%s|\n", i, actual_strarr[i], expected_strarr[i]);
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

				printf("  |%s| == |%s|\n", red_actual->filepath, red_expected->filepath);
				CHECK(red_actual->filepath && red_expected->filepath);
				CHECK_EQ_STR(red_actual->filepath, red_expected->filepath);
				printf("  |%d| == |%d|\n", red_actual->fd, red_expected->fd);
				CHECK_EQ(red_actual->fd, red_expected->fd);

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

				printf("  |%s| == |%s|\n", red_actual->filepath, red_expected->filepath);
				CHECK(red_actual->filepath && red_expected->filepath);
				CHECK_EQ_STR(red_actual->filepath, red_expected->filepath);
				printf("  |%d| == |%d|\n", red_actual->fd, red_expected->fd);
				CHECK_EQ(red_actual->fd, red_expected->fd);
				printf("  |%d| == |%d|\n", red_actual->is_append, red_expected->is_append);
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
	init_g_shell();

	TEST_CHAPTER("expand_env_var");

	TEST_SECTION("expand_env_var(hoge)");
	{
		char *input = ft_strdup("hoge");
		char *actual = expand_env_var(input);
		char *expected = "hoge";
		CHECK_EQ_STR(actual, expected);
		free(input);
		free(actual);
	}

	TEST_SECTION("expand_env_var($ABC)");
	{
		ft_setenv("ABC", "abc def", 0);
		char *input = ft_strdup("$ABC");
		char *actual = expand_env_var(input);
		char *expected = "abc def";
		CHECK_EQ_STR(actual, expected);
		free(input);
		free(actual);
		unsetenv("ABC");
	}

	TEST_SECTION("expand_env_var(hoge$ABC)");
	{
		ft_setenv("ABC", "abc def", 0);
		char *input = ft_strdup("hoge$ABC");
		char *actual = expand_env_var(input);
		char *expected = "hogeabc def";
		CHECK_EQ_STR(actual, expected);
		free(input);
		free(actual);
		unsetenv("ABC");
	}

	TEST_SECTION("expand_env_var(hoge$ABC) 環境変数の両端に空白がある");
	{
		ft_setenv("ABC", " abc def ", 0);
		char *input = ft_strdup("hoge$ABC");
		char *actual = expand_env_var(input);
		char *expected = "hoge abc def ";
		CHECK_EQ_STR(actual, expected);
		free(input);
		free(actual);
		unsetenv("ABC");
	}

	TEST_SECTION("expand_env_var($ABC-hoge)");
	{
		ft_setenv("ABC", "abc def", 0);
		char *input = ft_strdup("$ABC-hoge");
		char *actual = expand_env_var(input);
		char *expected = "abc def-hoge";
		CHECK_EQ_STR(actual, expected);
		free(input);
		free(actual);
		unsetenv("ABC");
	}

	TEST_SECTION("expand_env_var(hoge$ABC-abc)");
	{
		ft_setenv("ABC", "abc def", 0);
		char *input = ft_strdup("hoge$ABC-abc");
		char *actual = expand_env_var(input);
		char *expected = "hogeabc def-abc";
		CHECK_EQ_STR(actual, expected);
		free(input);
		free(actual);
		unsetenv("ABC");
	}

	TEST_SECTION("expand_env_var(\"$ABC\"\'\\\'$ABC\') ダブルクオーテーションとシングルクオーテーション");
	{
		ft_setenv("ABC", " abc def ", 0);
		char *input = ft_strdup("\"$ABC\"\'\\\'$ABC\'");
		char *actual = expand_env_var(input);
		char *expected = "\" abc def \"\'\\\'$ABC\'";
		printf("actual: %s\n", actual);
		CHECK_EQ_STR(actual, expected);
		free(input);
		free(actual);
		unsetenv("ABC");
	}

	TEST_SECTION("expand_env_var(\"$?\") 終了ステータス (0)\n");
	{
		set_status(0);
		char *input = ft_strdup("$?");
		char *actual = expand_env_var(input);
		char *expected = "0";
		CHECK_EQ_STR(actual, expected);
		free(input);
		free(actual);
	}

	TEST_SECTION("expand_env_var(\"$?\") 終了ステータス (-10)\n");
	{
		set_status(-10);
		char *input = ft_strdup("$?");
		char *actual = expand_env_var(input);
		char *expected = "-10";
		CHECK_EQ_STR(actual, expected);
		free(input);
		free(actual);
	}

	TEST_SECTION("expand_env_var(\"$?ABC\") 終了ステータスと文字列\n");
	{
		ft_setenv("ABC", "abc def", 0);
		set_status(0);
		char *input = ft_strdup("$?ABC");
		char *actual = expand_env_var(input);
		char *expected = "0ABC";
		CHECK_EQ_STR(actual, expected);
		free(input);
		free(actual);
		unsetenv("ABC");
	}

	TEST_SECTION("expand_env_var(\"ABC$?\") 文字列と終了ステータス\n");
	{
		set_status(0);
		char *input = ft_strdup("ABC$?");
		char *actual = expand_env_var(input);
		char *expected = "ABC0";
		CHECK_EQ_STR(actual, expected);
		free(input);
		free(actual);
	}

	TEST_SECTION("expand_env_var(\"$ABC?\") 終了ステータスは表示されない\n");
	{
		ft_setenv("ABC", "abc def", 0);
		set_status(0);
		char *input = ft_strdup("$ABC?");
		char *actual = expand_env_var(input);
		char *expected = "abc def?";
		CHECK_EQ_STR(actual, expected);
		free(input);
		free(actual);
		unsetenv("ABC");
	}

	TEST_SECTION("expand_env_var(\"$?$ABC\") 終了ステータスと環境変数\n");
	{
		ft_setenv("ABC", "abc def", 0);
		set_status(0);
		char *input = ft_strdup("$?$ABC");
		char *actual = expand_env_var(input);
		char *expected = "0abc def";
		CHECK_EQ_STR(actual, expected);
		free(input);
		free(actual);
		unsetenv("ABC");
	}

	/* bash での出力を元としたテストケース
	 *
	 * $ cat << HOGE
	 * > \$USER
	 * > "\$USER"
	 * > '$USER'
	 * > $USER
	 * > $\USER
	 * > \"\'\$\\
	 * $USER
	 * "$USER"
	 * 'jun'
	 * jun
	 * $\USER
	 * \"\'$\
	 */
	TEST_SECTION("expand_heredoc_document() 普通文字と各種記号とバックスラッシュ\n");
	{
		ft_setenv("USER", "jun", 0);
		set_status(0);
		char *input = ft_strdup(
			"\\$USER\n"
			"\"\\$USER\"\n"
			"'$USER'\n"
			"$USER\n"
			"$\\USER\n"
			"\\\"\\'\\$\\\\\n"
			);
		char *actual = expand_heredoc_document(input);
		char *expected =
			"$USER\n"
			"\"$USER\"\n"
			"'jun'\n"
			"jun\n"
			"$\\USER\n"
			"\\\"\\\'$\\\n";
		CHECK_EQ_STR(actual, expected);
		free(input);
		free(actual);
		unsetenv("ABC");
	}

	TEST_SECTION("string_node2string()");
	{
		/* 準備 */
		ft_setenv("ABC", "abc def", 0);
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "echo hoge$ABC\"hoge hoge\"'$ABC' \n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_command(&buf, &tok);
        CHECK_EQ(node->type, ASTNODE_COMMAND);
		CHECK_EQ(node->content.command->arguments_node->type, ASTNODE_ARGUMENTS);
		t_parse_node_arguments *args_node = node->content.command->arguments_node->content.arguments;
		CHECK_EQ(args_node->string_node->content.string->type, TOKTYPE_EXPANDABLE);
		CHECK_EQ_STR(args_node->string_node->content.string->text, "echo");
		args_node = args_node->rest_node->content.arguments;
		t_parse_node_string *string_node = args_node->string_node->content.string;
		CHECK_EQ(string_node->type, TOKTYPE_EXPANDABLE);
		CHECK_EQ_STR(string_node->text, "hoge");
		string_node = string_node->next->content.string;
		CHECK_EQ_STR(string_node->text, "$ABC");
		string_node = string_node->next->content.string;
		CHECK_EQ(string_node->type, TOKTYPE_EXPANDABLE_QUOTED);
		CHECK_EQ_STR(string_node->text, "hoge hoge");
		string_node = string_node->next->content.string;
		CHECK_EQ(string_node->type, TOKTYPE_NON_EXPANDABLE);
		CHECK_EQ_STR(string_node->text, "$ABC");
		char *expanded_str = string_node2string(args_node->string_node->content.string, true);
		printf("expanded: %s\n", expanded_str);
		CHECK_EQ_STR(expanded_str, "hoge$ABC\"hoge hoge\"'$ABC'");
		free(expanded_str);
		free(tok.text);
	}

	TEST_SECTION("expand_string_node() 環境変数とクオーテーションマーク");
	{
		/* 準備 */
		ft_setenv("ABC", "abc def", 0);
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "echo hoge$ABC\"hoge hoge\"'$ABC' \n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_command(&buf, &tok);
		CHECK_EQ(node->type, ASTNODE_COMMAND);
		CHECK_EQ(node->content.command->arguments_node->type, ASTNODE_ARGUMENTS);
		t_parse_node_arguments *args_node = node->content.command->arguments_node->content.arguments;
		CHECK_EQ(args_node->string_node->content.string->type, TOKTYPE_EXPANDABLE);
		CHECK_EQ_STR(args_node->string_node->content.string->text, "echo");

		args_node = args_node->rest_node->content.arguments;

		t_parse_node_string *string_node = args_node->string_node->content.string;
		CHECK_EQ(string_node->type, TOKTYPE_EXPANDABLE);
		CHECK_EQ_STR(string_node->text, "hoge");
		string_node = string_node->next->content.string;
		CHECK_EQ_STR(string_node->text, "$ABC");

		string_node = string_node->next->content.string;
		CHECK_EQ(string_node->type, TOKTYPE_EXPANDABLE_QUOTED);
		CHECK_EQ_STR(string_node->text, "hoge hoge");

		string_node = string_node->next->content.string;
		CHECK_EQ(string_node->type, TOKTYPE_NON_EXPANDABLE);
		CHECK_EQ_STR(string_node->text, "$ABC");

		char **actual = expand_string_node(args_node->string_node->content.string, false);
		char **expected = NULL;
		char **tmp = expected;
		expected = (char **)ptrarr_add_ptr((void **)expected, ft_strdup("hogeabc"));
		free(tmp);
		tmp = expected;
		expected = (char **)ptrarr_add_ptr((void **)expected, ft_strdup("defhoge hoge$ABC"));
		free(tmp);

		check_strarr((const char **)actual, (const char **)expected);
		free_ptrarr((void **)actual);
		free_ptrarr((void **)expected);
		free(tok.text);
	}

	TEST_SECTION("expand_string_node() エスケープされた環境変数");
	{
		/* 準備 */
		ft_setenv("ABC", " abc def ", 0);
		t_parse_buffer	buf;
		// echo "\$\$ABC\\$ABC""$ABC"
		init_buf_with_string(&buf, "echo \"\\$\\$ABC\\\\$ABC\"\"$ABC\" \n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_command(&buf, &tok);
		CHECK_EQ(node->type, ASTNODE_COMMAND);
		CHECK_EQ(node->content.command->arguments_node->type, ASTNODE_ARGUMENTS);
		t_parse_node_arguments *args_node = node->content.command->arguments_node->content.arguments;
		CHECK_EQ(args_node->string_node->content.string->type, TOKTYPE_EXPANDABLE);
		CHECK_EQ_STR(args_node->string_node->content.string->text, "echo");

		args_node = args_node->rest_node->content.arguments;

		t_parse_node_string *string_node = args_node->string_node->content.string;
		CHECK_EQ(string_node->type, TOKTYPE_NON_EXPANDABLE);
		CHECK_EQ_STR(string_node->text, "$");

		string_node = string_node->next->content.string;
		CHECK_EQ(string_node->type, TOKTYPE_NON_EXPANDABLE);
		CHECK_EQ_STR(string_node->text, "$");

		string_node = string_node->next->content.string;
		CHECK_EQ(string_node->type, TOKTYPE_EXPANDABLE_QUOTED);
		CHECK_EQ_STR(string_node->text, "ABC");

		string_node = string_node->next->content.string;
		CHECK_EQ(string_node->type, TOKTYPE_NON_EXPANDABLE);
		CHECK_EQ_STR(string_node->text, "\\");

		string_node = string_node->next->content.string;
		CHECK_EQ(string_node->type, TOKTYPE_EXPANDABLE_QUOTED);
		CHECK_EQ_STR(string_node->text, "$ABC");

		string_node = string_node->next->content.string;
		CHECK_EQ(string_node->type, TOKTYPE_EXPANDABLE_QUOTED);
		CHECK_EQ_STR(string_node->text, "$ABC");

		char **actual = expand_string_node(args_node->string_node->content.string, false);
		char **expected = NULL;
		char **tmp = expected;
		expected = (char **)ptrarr_add_ptr((void **)expected, ft_strdup("$$ABC\\ abc def  abc def "));
		free(tmp);

		check_strarr((const char **)actual, (const char **)expected);
		free_ptrarr((void **)actual);
		free_ptrarr((void **)expected);
		free(tok.text);
	}

	TEST_SECTION("expand_string_node() exportコマンドの時の空白区切り");
	{
		/* 準備 */
		ft_setenv("b", "a  b", 0);
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "export abc=\"a\"$b\"c\"\n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_command(&buf, &tok);
		CHECK_EQ(node->type, ASTNODE_COMMAND);
		CHECK_EQ(node->content.command->arguments_node->type, ASTNODE_ARGUMENTS);
		t_parse_node_arguments *args_node = node->content.command->arguments_node->content.arguments;
		CHECK_EQ(args_node->string_node->content.string->type, TOKTYPE_EXPANDABLE);
		CHECK_EQ_STR(args_node->string_node->content.string->text, "export");

		args_node = args_node->rest_node->content.arguments;

		t_parse_node_string *string_node = args_node->string_node->content.string;
		CHECK_EQ(string_node->type, TOKTYPE_EXPANDABLE);
		CHECK_EQ_STR(string_node->text, "abc=");
		string_node = string_node->next->content.string;
		CHECK_EQ(string_node->type, TOKTYPE_EXPANDABLE_QUOTED);
		CHECK_EQ_STR(string_node->text, "a");
		string_node = string_node->next->content.string;
		CHECK_EQ(string_node->type, TOKTYPE_EXPANDABLE);
		CHECK_EQ_STR(string_node->text, "$b");
		string_node = string_node->next->content.string;
		CHECK_EQ(string_node->type, TOKTYPE_EXPANDABLE_QUOTED);
		CHECK_EQ_STR(string_node->text, "c");

		char **actual = expand_string_node(args_node->string_node->content.string, true);
		char **expected = NULL;
		char **tmp = expected;
		expected = (char **)ptrarr_add_ptr((void **)expected, ft_strdup("abc=aa  bc"));
		free(tmp);

		check_strarr((const char **)actual, (const char **)expected);
		free_ptrarr((void **)actual);
		free_ptrarr((void **)expected);
		free(tok.text);
	}

	TEST_CHAPTER("AST to command_invocation");

	TEST_SECTION("cmd_ast_cmd2cmdinvo 文字列1つ");
	{
		/* 準備 */
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "abc \n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_command(&buf, &tok);
        CHECK_EQ(node->type, ASTNODE_COMMAND);
		t_parse_node_arguments *args_node = node->content.command->arguments_node->content.arguments;
		t_parse_node_string *string_node = args_node->string_node->content.string;
		CHECK_EQ_STR(string_node->text, "abc");

		/* テスト */
		t_command_invocation *actual = cmd_ast_cmd2cmdinvo(node->content.command);
		t_command_invocation *expected = cmd_init_cmdinvo((const char **)ft_split("abc", ' '));
		CHECK(actual);
		check_cmdinvo(actual, expected);

		cmd_free_cmdinvo(actual);
		cmd_free_cmdinvo(expected);
		free(tok.text);
	}

	TEST_SECTION("cmd_ast_cmd2cmdinvo arguments 2個");
	{
		/* 準備 */
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "abc def\n");
		t_token	tok;
		lex_init_token(&tok);

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
		free(tok.text);
	}

	TEST_SECTION("cmd_ast_cmd2cmdinvo() 空文字列を含む");
	{
		/* 準備 */
		t_parse_buffer	buf;
		// echo a "" b "" c
		init_buf_with_string(&buf, "echo a \"\" b \"\" c\n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_command(&buf, &tok);
		CHECK_EQ(node->type, ASTNODE_COMMAND);
		CHECK_EQ(node->content.command->arguments_node->type, ASTNODE_ARGUMENTS);
		t_parse_node_arguments *args_node = node->content.command->arguments_node->content.arguments;
		CHECK_EQ(args_node->string_node->content.string->type, TOKTYPE_EXPANDABLE);
		CHECK_EQ_STR(args_node->string_node->content.string->text, "echo");

		args_node = args_node->rest_node->content.arguments;

		t_parse_node_string *string_node = args_node->string_node->content.string;
		CHECK_EQ(string_node->type, TOKTYPE_EXPANDABLE);
		CHECK_EQ_STR(string_node->text, "a");

		args_node = args_node->rest_node->content.arguments;
		string_node = args_node->string_node->content.string;
		CHECK_EQ(string_node->type, TOKTYPE_EXPANDABLE_QUOTED);
		CHECK_EQ_STR(string_node->text, "");

		args_node = args_node->rest_node->content.arguments;
		string_node = args_node->string_node->content.string;
		CHECK_EQ(string_node->type, TOKTYPE_EXPANDABLE);
		CHECK_EQ_STR(string_node->text, "b");

		args_node = args_node->rest_node->content.arguments;
		string_node = args_node->string_node->content.string;
		CHECK_EQ(string_node->type, TOKTYPE_EXPANDABLE_QUOTED);
		CHECK_EQ_STR(string_node->text, "");

		args_node = args_node->rest_node->content.arguments;
		string_node = args_node->string_node->content.string;
		CHECK_EQ(string_node->type, TOKTYPE_EXPANDABLE);
		CHECK_EQ_STR(string_node->text, "c");

		/* テスト */
		t_command_invocation *actual = cmd_ast_cmd2cmdinvo(node->content.command);
		t_command_invocation *expected = cmd_init_cmdinvo(NULL);

		const char **tmp;
		tmp = expected->exec_and_args;
		expected->exec_and_args = (const char**)ptrarr_add_ptr((void **)tmp, ft_strdup("echo"));
		free(tmp);
		tmp = expected->exec_and_args;
		expected->exec_and_args = (const char**)ptrarr_add_ptr((void **)tmp, ft_strdup("a"));
		free(tmp);
		tmp = expected->exec_and_args;
		expected->exec_and_args = (const char**)ptrarr_add_ptr((void **)tmp, ft_strdup(""));
		free(tmp);
		tmp = expected->exec_and_args;
		expected->exec_and_args = (const char**)ptrarr_add_ptr((void **)tmp, ft_strdup("b"));
		free(tmp);
		tmp = expected->exec_and_args;
		expected->exec_and_args = (const char**)ptrarr_add_ptr((void **)tmp, ft_strdup(""));
		free(tmp);
		tmp = expected->exec_and_args;
		expected->exec_and_args = (const char**)ptrarr_add_ptr((void **)tmp, ft_strdup("c"));
		free(tmp);

		CHECK(actual);
		check_cmdinvo(actual, expected);

		cmd_free_cmdinvo(actual);
		cmd_free_cmdinvo(expected);
		free(tok.text);
	}

	TEST_SECTION("cmd_ast_cmd2cmdinvo リダイレクト付き");
	{
		/* 準備 */
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "file < abc \n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_command(&buf, &tok);
        CHECK_EQ(node->type, ASTNODE_COMMAND);

		/* テスト */
		t_command_invocation *actual = cmd_ast_cmd2cmdinvo(node->content.command);
		t_command_invocation *expected = cmd_init_cmdinvo((const char **)ft_split("file", ' '));
		cmd_add_inredirect(expected, "abc", 0);
		CHECK(actual);
		check_cmdinvo(actual, expected);

		cmd_free_cmdinvo(actual);
		cmd_free_cmdinvo(expected);
		free(tok.text);
	}

	TEST_SECTION("cmd_ast_cmd2cmdinvo 入力リダイレクト, ディスクリプタ");
	{
		/* 準備 */
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "file 123< abc \n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_command(&buf, &tok);
        CHECK_EQ(node->type, ASTNODE_COMMAND);

		/* テスト */
		t_command_invocation *actual = cmd_ast_cmd2cmdinvo(node->content.command);
		t_command_invocation *expected = cmd_init_cmdinvo((const char **)ft_split("file", ' '));
		cmd_add_inredirect(expected, "abc", 123);
		CHECK(actual);
		check_cmdinvo(actual, expected);

		cmd_free_cmdinvo(actual);
		cmd_free_cmdinvo(expected);
		free(tok.text);
	}

	TEST_SECTION("cmd_ast_cmd2cmdinvo 出力リダイレクト付き");
	{
		/* 準備 */
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "file > abc \n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_command(&buf, &tok);
        CHECK_EQ(node->type, ASTNODE_COMMAND);

		/* テスト */
		t_command_invocation *actual = cmd_ast_cmd2cmdinvo(node->content.command);
		t_command_invocation *expected = cmd_init_cmdinvo((const char **)ft_split("file", ' '));
		cmd_add_outredirect(expected, "abc", 1, false);
		CHECK(actual);
		check_cmdinvo(actual, expected);

		cmd_free_cmdinvo(actual);
		cmd_free_cmdinvo(expected);
		free(tok.text);
	}

	TEST_SECTION("cmd_ast_cmd2cmdinvo 出力リダイレクト, ディスクリプタ");
	{
		/* 準備 */
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "file 123> abc \n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_command(&buf, &tok);
        CHECK_EQ(node->type, ASTNODE_COMMAND);

		/* テスト */
		t_command_invocation *actual = cmd_ast_cmd2cmdinvo(node->content.command);
		t_command_invocation *expected = cmd_init_cmdinvo((const char **)ft_split("file", ' '));
		cmd_add_outredirect(expected, "abc", 123, false);
		CHECK(actual);
		check_cmdinvo(actual, expected);

		cmd_free_cmdinvo(actual);
		cmd_free_cmdinvo(expected);
		free(tok.text);
	}

	TEST_SECTION("cmd_ast_cmd2cmdinvo 入出力リダイレクト付き");
	{
		/* 準備 */
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "file < input > output \n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_command(&buf, &tok);
        CHECK_EQ(node->type, ASTNODE_COMMAND);

		/* テスト */
		t_command_invocation *actual = cmd_ast_cmd2cmdinvo(node->content.command);
		t_command_invocation *expected = cmd_init_cmdinvo((const char **)ft_split("file", ' '));
		cmd_add_inredirect(expected, "input", 0);
		cmd_add_outredirect(expected, "output", 1, false);
		CHECK(actual);
		check_cmdinvo(actual, expected);

		cmd_free_cmdinvo(actual);
		cmd_free_cmdinvo(expected);
		free(tok.text);
	}

	TEST_SECTION("cmd_ast_cmd2cmdinvo 追記リダイレクト付き");
	{
		/* 準備 */
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "file >> abc \n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_command(&buf, &tok);
        CHECK_EQ(node->type, ASTNODE_COMMAND);

		/* テスト */
		t_command_invocation *actual = cmd_ast_cmd2cmdinvo(node->content.command);
		t_command_invocation *expected = cmd_init_cmdinvo((const char **)ft_split("file", ' '));
		cmd_add_outredirect(expected, "abc", 1, true);
		CHECK(actual);
		check_cmdinvo(actual, expected);

		cmd_free_cmdinvo(actual);
		cmd_free_cmdinvo(expected);
		free(tok.text);
	}

	TEST_SECTION("cmd_ast_cmd2cmdinvo 追記リダイレクト, ディスクリプタ");
	{
		/* 準備 */
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "file 456>> abc \n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_command(&buf, &tok);
        CHECK_EQ(node->type, ASTNODE_COMMAND);

		/* テスト */
		t_command_invocation *actual = cmd_ast_cmd2cmdinvo(node->content.command);
		t_command_invocation *expected = cmd_init_cmdinvo((const char **)ft_split("file", ' '));
		cmd_add_outredirect(expected, "abc", 456, true);
		CHECK(actual);
		check_cmdinvo(actual, expected);

		cmd_free_cmdinvo(actual);
		cmd_free_cmdinvo(expected);
		free(tok.text);
	}


	TEST_SECTION("cmd_ast_cmd2cmdinvo 複数入力リダイレクト");
	{
		/* 準備 */
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "file < a < b < c < d \n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_command(&buf, &tok);
        CHECK_EQ(node->type, ASTNODE_COMMAND);

		/* テスト */
		t_command_invocation *actual = cmd_ast_cmd2cmdinvo(node->content.command);
		t_command_invocation *expected = cmd_init_cmdinvo((const char **)ft_split("file", ' '));
		cmd_add_inredirect(expected, "a", 0);
		cmd_add_inredirect(expected, "b", 0);
		cmd_add_inredirect(expected, "c", 0);
		cmd_add_inredirect(expected, "d", 0);
		CHECK(actual);
		check_cmdinvo(actual, expected);

		cmd_free_cmdinvo(actual);
		cmd_free_cmdinvo(expected);
		free(tok.text);
	}

	TEST_SECTION("cmd_ast_cmd2cmdinvo 複数書き込みリダイレクト");
	{
		/* 準備 */
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "file > a > b > c > d \n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_command(&buf, &tok);
        CHECK_EQ(node->type, ASTNODE_COMMAND);

		/* テスト */
		t_command_invocation *actual = cmd_ast_cmd2cmdinvo(node->content.command);
		t_command_invocation *expected = cmd_init_cmdinvo((const char **)ft_split("file", ' '));
		cmd_add_outredirect(expected, "a", 1, false);
		cmd_add_outredirect(expected, "b", 1, false);
		cmd_add_outredirect(expected, "c", 1, false);
		cmd_add_outredirect(expected, "d", 1, false);
		CHECK(actual);
		check_cmdinvo(actual, expected);

		cmd_free_cmdinvo(actual);
		cmd_free_cmdinvo(expected);
		free(tok.text);
	}

	TEST_SECTION("cmd_ast_cmd2cmdinvo 複数追記リダイレクト");
	{
		/* 準備 */
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "file >> a >> b >> c >> d \n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_command(&buf, &tok);
        CHECK_EQ(node->type, ASTNODE_COMMAND);

		/* テスト */
		t_command_invocation *actual = cmd_ast_cmd2cmdinvo(node->content.command);
		t_command_invocation *expected = cmd_init_cmdinvo((const char **)ft_split("file", ' '));
		cmd_add_outredirect(expected, "a", 1, true);
		cmd_add_outredirect(expected, "b", 1, true);
		cmd_add_outredirect(expected, "c", 1, true);
		cmd_add_outredirect(expected, "d", 1, true);
		CHECK(actual);
		check_cmdinvo(actual, expected);

		cmd_free_cmdinvo(actual);
		cmd_free_cmdinvo(expected);
		free(tok.text);
	}

	TEST_SECTION("cmd_ast_cmd2cmdinvo 複数入力リダイレクト");
	{
		/* 準備 */
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "file < a < b > c > d \n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_command(&buf, &tok);
        CHECK_EQ(node->type, ASTNODE_COMMAND);

		/* テスト */
		t_command_invocation *actual = cmd_ast_cmd2cmdinvo(node->content.command);
		t_command_invocation *expected = cmd_init_cmdinvo((const char **)ft_split("file", ' '));
		cmd_add_inredirect(expected, "a", 0);
		cmd_add_inredirect(expected, "b", 0);
		cmd_add_outredirect(expected, "c", 1, false);
		cmd_add_outredirect(expected, "d", 1, false);
		CHECK(actual);
		check_cmdinvo(actual, expected);

		cmd_free_cmdinvo(actual);
		cmd_free_cmdinvo(expected);
		free(tok.text);
	}

	TEST_SECTION("cmd_ast_cmd2cmdinvo 環境変数");
	{
		/* 準備 */
		ft_setenv("ABC", "abc", 0);
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "echo $ABC \n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_command(&buf, &tok);
        CHECK_EQ(node->type, ASTNODE_COMMAND);
		CHECK_EQ(node->content.command->arguments_node->type, ASTNODE_ARGUMENTS);
		t_parse_node_arguments *args_node = node->content.command->arguments_node->content.arguments;
		CHECK_EQ(args_node->string_node->content.string->type, TOKTYPE_EXPANDABLE);
		CHECK_EQ_STR(args_node->string_node->content.string->text, "echo");
		args_node = args_node->rest_node->content.arguments;
		CHECK_EQ(args_node->string_node->content.string->type, TOKTYPE_EXPANDABLE);
		CHECK_EQ_STR(args_node->string_node->content.string->text, "$ABC");

		/* テスト */
		t_command_invocation *actual = cmd_ast_cmd2cmdinvo(node->content.command);
		t_command_invocation *expected = cmd_init_cmdinvo((const char **)ft_split("echo abc", ' '));
		CHECK(actual);
		check_cmdinvo(actual, expected);

		cmd_free_cmdinvo(actual);
		cmd_free_cmdinvo(expected);
		unsetenv("ABC");
		free(tok.text);
	}

	TEST_SECTION("cmd_ast_cmd2cmdinvo ダブルクオーテーションで囲まれた環境変数");
	{
		/* 準備 */
		ft_setenv("ABC", "abc", 0);
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "echo \"$ABC\" \n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_command(&buf, &tok);
        CHECK_EQ(node->type, ASTNODE_COMMAND);
		CHECK_EQ(node->content.command->arguments_node->type, ASTNODE_ARGUMENTS);
		t_parse_node_arguments *args_node = node->content.command->arguments_node->content.arguments;
		CHECK_EQ(args_node->string_node->content.string->type, TOKTYPE_EXPANDABLE);
		CHECK_EQ_STR(args_node->string_node->content.string->text, "echo");
		args_node = args_node->rest_node->content.arguments;
		CHECK_EQ(args_node->string_node->content.string->type, TOKTYPE_EXPANDABLE_QUOTED);
		CHECK_EQ_STR(args_node->string_node->content.string->text, "$ABC");

		/* テスト */
		t_command_invocation *actual = cmd_ast_cmd2cmdinvo(node->content.command);
		t_command_invocation *expected = cmd_init_cmdinvo((const char **)ft_split("echo abc", ' '));
		CHECK(actual);
		check_cmdinvo(actual, expected);

		cmd_free_cmdinvo(actual);
		cmd_free_cmdinvo(expected);
		unsetenv("ABC");
		free(tok.text);
	}

	TEST_SECTION("cmd_ast_cmd2cmdinvo シングルクオーテーションで囲まれた環境変数");
	{
		/* 準備 */
		ft_setenv("ABC", "abc", 0);
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "echo \'$ABC\' \n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_command(&buf, &tok);
        CHECK_EQ(node->type, ASTNODE_COMMAND);
		CHECK_EQ(node->content.command->arguments_node->type, ASTNODE_ARGUMENTS);
		t_parse_node_arguments *args_node = node->content.command->arguments_node->content.arguments;
		CHECK_EQ(args_node->string_node->content.string->type, TOKTYPE_EXPANDABLE);
		CHECK_EQ_STR(args_node->string_node->content.string->text, "echo");
		args_node = args_node->rest_node->content.arguments;
		CHECK_EQ(args_node->string_node->content.string->type, TOKTYPE_NON_EXPANDABLE);
		CHECK_EQ_STR(args_node->string_node->content.string->text, "$ABC");

		/* テスト */
		t_command_invocation *actual = cmd_ast_cmd2cmdinvo(node->content.command);
		t_command_invocation *expected = cmd_init_cmdinvo((const char **)ft_split("echo $ABC", ' '));
		CHECK(actual);
		check_cmdinvo(actual, expected);

		cmd_free_cmdinvo(actual);
		cmd_free_cmdinvo(expected);
		unsetenv("ABC");
		free(tok.text);
	}


	TEST_SECTION("cmd_ast_cmd2cmdinvo スペースが含まれている環境変数");
	{
		/* 準備 */
		ft_setenv("ABC", "abc def", 0);
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "echo $ABC \n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_command(&buf, &tok);
        CHECK_EQ(node->type, ASTNODE_COMMAND);
		CHECK_EQ(node->content.command->arguments_node->type, ASTNODE_ARGUMENTS);
		t_parse_node_arguments *args_node = node->content.command->arguments_node->content.arguments;
		CHECK_EQ(args_node->string_node->content.string->type, TOKTYPE_EXPANDABLE);
		CHECK_EQ_STR(args_node->string_node->content.string->text, "echo");
		args_node = args_node->rest_node->content.arguments;
		CHECK_EQ(args_node->string_node->content.string->type, TOKTYPE_EXPANDABLE);
		CHECK_EQ_STR(args_node->string_node->content.string->text, "$ABC");

		/* テスト */
		t_command_invocation *actual = cmd_ast_cmd2cmdinvo(node->content.command);
		t_command_invocation *expected = cmd_init_cmdinvo((const char **)ft_split("echo abc def", ' '));
		CHECK(actual);
		check_cmdinvo(actual, expected);

		cmd_free_cmdinvo(actual);
		cmd_free_cmdinvo(expected);
		unsetenv("ABC");
		free(tok.text);
	}

	TEST_SECTION("cmd_ast_cmd2cmdinvo 文字列と環境変数");
	{
		/* 準備 */
		ft_setenv("ABC", "abc def", 0);
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "echo hoge$ABC \n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_command(&buf, &tok);
        CHECK_EQ(node->type, ASTNODE_COMMAND);
		CHECK_EQ(node->content.command->arguments_node->type, ASTNODE_ARGUMENTS);
		t_parse_node_arguments *args_node = node->content.command->arguments_node->content.arguments;
		CHECK_EQ(args_node->string_node->content.string->type, TOKTYPE_EXPANDABLE);
		CHECK_EQ_STR(args_node->string_node->content.string->text, "echo");
		args_node = args_node->rest_node->content.arguments;
		CHECK_EQ(args_node->string_node->content.string->type, TOKTYPE_EXPANDABLE);
		t_parse_node_string *string_node = args_node->string_node->content.string;
		CHECK_EQ_STR(string_node->text, "hoge");
		string_node = string_node->next->content.string;
		CHECK_EQ_STR(string_node->text, "$ABC");

		/* テスト */
		t_command_invocation *actual = cmd_ast_cmd2cmdinvo(node->content.command);
		t_command_invocation *expected = cmd_init_cmdinvo((const char **)ft_split("echo hogeabc def", ' '));
		CHECK(actual);
		check_cmdinvo(actual, expected);

		cmd_free_cmdinvo(actual);
		cmd_free_cmdinvo(expected);
		unsetenv("ABC");
		free(tok.text);
	}


	TEST_SECTION("cmd_ast_cmd2cmdinvo スペースが含まれている環境変数とダブルクオーテーション");
	{
		/* 準備 */
		ft_setenv("ABC", "abc def", 0);
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "echo $ABC\"ghi jkl\" \n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_command(&buf, &tok);
        CHECK_EQ(node->type, ASTNODE_COMMAND);
		CHECK_EQ(node->content.command->arguments_node->type, ASTNODE_ARGUMENTS);
		t_parse_node_arguments *args_node = node->content.command->arguments_node->content.arguments;
		CHECK_EQ(args_node->string_node->content.string->type, TOKTYPE_EXPANDABLE);
		CHECK_EQ_STR(args_node->string_node->content.string->text, "echo");
		args_node = args_node->rest_node->content.arguments;
		t_parse_node_string *string_node = args_node->string_node->content.string;
		CHECK_EQ(string_node->type, TOKTYPE_EXPANDABLE);
		CHECK_EQ_STR(string_node->text, "$ABC");
		string_node = string_node->next->content.string;
		CHECK_EQ(string_node->type, TOKTYPE_EXPANDABLE_QUOTED);
		CHECK_EQ_STR(string_node->text, "ghi jkl");

		/* テスト */
		t_command_invocation *actual = cmd_ast_cmd2cmdinvo(node->content.command);
		t_command_invocation *expected = cmd_init_cmdinvo((const char **)ft_split("echo abc", ' '));
		const char **strarr = (const char**)ptrarr_add_ptr((void **)expected->exec_and_args, ft_strdup("defghi jkl"));
		free(expected->exec_and_args);
		expected->exec_and_args = strarr;
		CHECK(actual);
		check_cmdinvo(actual, expected);

		cmd_free_cmdinvo(actual);
		cmd_free_cmdinvo(expected);
		unsetenv("ABC");
		free(tok.text);
	}

	TEST_SECTION("cmd_ast_cmd2cmdinvo 環境変数が複数");
	{
		/* 準備 */
		ft_setenv("ABC", "abc def", 0);
		ft_setenv("DEF", "ABC DEF", 0);
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "echo $ABC$DEF \n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_command(&buf, &tok);
        CHECK_EQ(node->type, ASTNODE_COMMAND);
		CHECK_EQ(node->content.command->arguments_node->type, ASTNODE_ARGUMENTS);
		t_parse_node_arguments *args_node = node->content.command->arguments_node->content.arguments;
		CHECK_EQ(args_node->string_node->content.string->type, TOKTYPE_EXPANDABLE);
		CHECK_EQ_STR(args_node->string_node->content.string->text, "echo");
		args_node = args_node->rest_node->content.arguments;
		CHECK_EQ(args_node->string_node->content.string->type, TOKTYPE_EXPANDABLE);

		t_parse_node_string *string_node = args_node->string_node->content.string;

		CHECK_EQ_STR(string_node->text, "$ABC");
		string_node = string_node->next->content.string;
		CHECK_EQ_STR(string_node->text, "$DEF");

		/* テスト */
		t_command_invocation *actual = cmd_ast_cmd2cmdinvo(node->content.command);
		t_command_invocation *expected = cmd_init_cmdinvo((const char **)ft_split("echo abc defABC DEF", ' '));
		CHECK(actual);
		check_cmdinvo(actual, expected);

		cmd_free_cmdinvo(actual);
		cmd_free_cmdinvo(expected);
		unsetenv("ABC");
		unsetenv("DEF");
		free(tok.text);
	}

	TEST_SECTION("cmd_ast_cmd2cmdinvo リダイレクション 環境変数");
	{
		/* 準備 */
		ft_setenv("ABC", "abc", 0);
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "echo hello > $ABC \n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_command(&buf, &tok);
        CHECK_EQ(node->type, ASTNODE_COMMAND);
		CHECK_EQ(node->content.command->arguments_node->type, ASTNODE_ARGUMENTS);
		t_parse_node_arguments *args_node = node->content.command->arguments_node->content.arguments;
		CHECK_EQ(args_node->string_node->content.string->type, TOKTYPE_EXPANDABLE);
		CHECK_EQ_STR(args_node->string_node->content.string->text, "echo");
		args_node = args_node->rest_node->content.arguments;
		CHECK_EQ(args_node->string_node->content.string->type, TOKTYPE_EXPANDABLE);
		CHECK_EQ_STR(args_node->string_node->content.string->text, "hello");
		args_node = args_node->rest_node->content.arguments;
		CHECK_EQ(args_node->redirection_node->content.redirection->type, TOKTYPE_OUTPUT_REDIRECTION);
		CHECK_EQ_STR(args_node->redirection_node->content.redirection->string_node->content.string->text, "$ABC");

		/* テスト */
		t_command_invocation *actual = cmd_ast_cmd2cmdinvo(node->content.command);
		t_command_invocation *expected = cmd_init_cmdinvo((const char **)ft_split("echo hello", ' '));
		cmd_add_outredirect(expected, "$ABC", 1, false);
		CHECK(actual);
		check_cmdinvo(actual, expected);

		cmd_free_cmdinvo(actual);
		cmd_free_cmdinvo(expected);
		unsetenv("ABC");
		free(tok.text);
	}

	TEST_SECTION("cmd_ast_cmd2cmdinvo リダイレクション 環境変数と文字列");
	{
		/* 準備 */
		ft_setenv("ABC", "abc", 0);
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "echo hello > hoge$ABC \n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_command(&buf, &tok);
        CHECK_EQ(node->type, ASTNODE_COMMAND);
		CHECK_EQ(node->content.command->arguments_node->type, ASTNODE_ARGUMENTS);
		t_parse_node_arguments *args_node = node->content.command->arguments_node->content.arguments;
		CHECK_EQ(args_node->string_node->content.string->type, TOKTYPE_EXPANDABLE);
		CHECK_EQ_STR(args_node->string_node->content.string->text, "echo");
		args_node = args_node->rest_node->content.arguments;
		CHECK_EQ(args_node->string_node->content.string->type, TOKTYPE_EXPANDABLE);
		CHECK_EQ_STR(args_node->string_node->content.string->text, "hello");
		args_node = args_node->rest_node->content.arguments;
		CHECK_EQ(args_node->redirection_node->content.redirection->type, TOKTYPE_OUTPUT_REDIRECTION);

		t_parse_node_string *string_node =
			args_node->redirection_node->content.redirection
			->string_node->content.string;
		CHECK_EQ_STR(string_node->text, "hoge");
		string_node = string_node->next->content.string;
		CHECK_EQ_STR(string_node->text, "$ABC");

		/* テスト */
		t_command_invocation *actual = cmd_ast_cmd2cmdinvo(node->content.command);
		t_command_invocation *expected = cmd_init_cmdinvo((const char **)ft_split("echo hello", ' '));
		cmd_add_outredirect(expected, "hoge$ABC", 1, false);
		CHECK(actual);
		check_cmdinvo(actual, expected);

		cmd_free_cmdinvo(actual);
		cmd_free_cmdinvo(expected);
		unsetenv("ABC");
		free(tok.text);
	}

	TEST_SECTION("cmd_ast_cmd2cmdinvo 存在しない環境変数");
	{
		/* 準備 */
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "echo $DONTEXISTS \n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_command(&buf, &tok);
        CHECK_EQ(node->type, ASTNODE_COMMAND);
		CHECK_EQ(node->content.command->arguments_node->type, ASTNODE_ARGUMENTS);
		t_parse_node_arguments *args_node = node->content.command->arguments_node->content.arguments;
		CHECK_EQ(args_node->string_node->content.string->type, TOKTYPE_EXPANDABLE);
		CHECK_EQ_STR(args_node->string_node->content.string->text, "echo");
		args_node = args_node->rest_node->content.arguments;
		CHECK_EQ(args_node->string_node->content.string->type, TOKTYPE_EXPANDABLE);
		CHECK_EQ_STR(args_node->string_node->content.string->text, "$DONTEXISTS");

		/* テスト */
		t_command_invocation *actual = cmd_ast_cmd2cmdinvo(node->content.command);
		t_command_invocation *expected = cmd_init_cmdinvo((const char **)ft_split("echo", ' '));
		CHECK(actual);
		check_cmdinvo(actual, expected);

		cmd_free_cmdinvo(actual);
		cmd_free_cmdinvo(expected);
		free(tok.text);
	}

	TEST_SECTION("cmd_ast_pipcmds2cmdinvo 文字列1つ");
	{
		/* 準備 */
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "abc \n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_piped_commands(&buf, &tok);
        CHECK_EQ(node->type, ASTNODE_PIPED_COMMANDS);
		t_parse_node_arguments *args_node = node->content.piped_commands->command_node->content.command->arguments_node->content.arguments;
		t_parse_node_string *string_node = args_node->string_node->content.string;
		CHECK_EQ_STR(string_node->text, "abc");

		/* テスト */
		t_command_invocation *actual = cmd_ast_pipcmds2cmdinvo(node->content.piped_commands);
		t_command_invocation *expected = cmd_init_cmdinvo((const char **)ft_split("abc", ' '));
		CHECK(actual);
		check_cmdinvo(actual, expected);

		cmd_free_cmdinvo(actual);
		cmd_free_cmdinvo(expected);
		free(tok.text);
	}

	TEST_SECTION("cmd_ast_pipcmds2cmdinvo arguments 2個");
	{
		/* 準備 */
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "abc def\n");
		t_token	tok;
		lex_init_token(&tok);

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
		free(tok.text);
	}

	TEST_SECTION("cmd_ast_pipcmds2cmdinvo パイプ & リダイレクション");
	{
		/* 準備 */
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "abc | file < abc \n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_piped_commands(&buf, &tok);

		/* テスト */
		t_command_invocation *actual = cmd_ast_pipcmds2cmdinvo(node->content.piped_commands);
		t_command_invocation *expected_first = cmd_init_cmdinvo((const char **)ft_split("abc", ' '));
		t_command_invocation *expected_second = cmd_init_cmdinvo((const char **)ft_split("file", ' '));
		cmd_add_inredirect(expected_second, "abc", 0);
		expected_first->piped_command = expected_second;

		CHECK(actual);
		check_cmdinvo(actual, expected_first);

		cmd_free_cmdinvo(actual);
		cmd_free_cmdinvo(expected_first);
		free(tok.text);
	}

	TEST_SECTION("cmd_ast_pipcmds2cmdinvo パイプ & 出力リダイレクション");
	{
		/* 準備 */
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "abc | file > abc \n");
		t_token	tok;
		lex_init_token(&tok);

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_piped_commands(&buf, &tok);

		/* テスト */
		t_command_invocation *actual = cmd_ast_pipcmds2cmdinvo(node->content.piped_commands);
		t_command_invocation *expected_first = cmd_init_cmdinvo((const char **)ft_split("abc", ' '));
		t_command_invocation *expected_second = cmd_init_cmdinvo((const char **)ft_split("file", ' '));
		cmd_add_outredirect(expected_second, "abc", 1, false);
		expected_first->piped_command = expected_second;

		CHECK(actual);
		check_cmdinvo(actual, expected_first);

		cmd_free_cmdinvo(actual);
		cmd_free_cmdinvo(expected_first);
		free(tok.text);
	}

	free_vars(g_shell.vars);
	parse_free_all_ast();
	int fail_count = print_result();
	return (fail_count);
}
