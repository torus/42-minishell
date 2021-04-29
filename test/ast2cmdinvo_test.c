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
		setenv("ABC", "abc def", 1);
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
		setenv("ABC", "abc def", 1);
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
		setenv("ABC", " abc def ", 1);
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
		setenv("ABC", "abc def", 1);
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
		setenv("ABC", "abc def", 1);
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
		setenv("ABC", " abc def ", 1);
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
		char	*input = ft_strdup("$?");
		char *actual = expand_env_var(input);
		char *expected = "-10";
		CHECK_EQ_STR(actual, expected);
		free(input);
		free(actual);
	}

	TEST_SECTION("expand_env_var(\"$?ABC\") 終了ステータスと文字列\n");
	{
		setenv("ABC", "abc def", 1);
		set_status(0);
		char	*input = ft_strdup("$?ABC");
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
		char	*input = ft_strdup("ABC$?");
		char *actual = expand_env_var(input);
		char *expected = "ABC0";
		CHECK_EQ_STR(actual, expected);
		free(input);
		free(actual);
	}

	TEST_SECTION("expand_env_var(\"$ABC?\") 終了ステータスは表示されない\n");
	{
		setenv("ABC", "abc def", 1);
		set_status(0);
		char	*input = ft_strdup("$ABC?");
		char *actual = expand_env_var(input);
		char *expected = "abc def?";
		CHECK_EQ_STR(actual, expected);
		free(input);
		free(actual);
		unsetenv("ABC");
	}

	TEST_SECTION("expand_env_var(\"$?$ABC\") 終了ステータスと環境変数\n");
	{
		setenv("ABC", "abc def", 1);
		set_status(0);
		char	*input = ft_strdup("$?$ABC");
		char *actual = expand_env_var(input);
		char *expected = "0abc def";
		CHECK_EQ_STR(actual, expected);
		free(input);
		free(actual);
		unsetenv("ABC");
	}

	// TODO: これでargs_nodeのコマンド引数展開がうまくいけそうならこの処理はparser側でやってもらう
	TEST_SECTION("string_node2string()");
	{
		/* 準備 */
		setenv("ABC", "abc def", 1);
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "echo hoge$ABC\"hoge hoge\"'$ABC' \n");
		t_token	tok;

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
		CHECK_EQ_STR(string_node->text, "hoge$ABC");
		string_node = string_node->next->content.string;
		CHECK_EQ(string_node->type, TOKTYPE_EXPANDABLE_QUOTED);
		CHECK_EQ_STR(string_node->text, "hoge hoge");
		string_node = string_node->next->content.string;
		CHECK_EQ(string_node->type, TOKTYPE_NON_EXPANDABLE);
		CHECK_EQ_STR(string_node->text, "$ABC");
		char *expanded_str = string_node2string(args_node->string_node->content.string);
		printf("expanded: %s\n", expanded_str);
		CHECK_EQ_STR(expanded_str, "hoge$ABC\"hoge hoge\"'$ABC'");
		free(expanded_str);
	}

	TEST_SECTION("expand_string_node() 環境変数とクオーテーションマーク");
	{
		/* 準備 */
		setenv("ABC", "abc def", 1);
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "echo hoge$ABC\"hoge hoge\"'$ABC' \n");
		t_token	tok;

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
		CHECK_EQ_STR(string_node->text, "hoge$ABC");
		string_node = string_node->next->content.string;
		CHECK_EQ(string_node->type, TOKTYPE_EXPANDABLE_QUOTED);
		CHECK_EQ_STR(string_node->text, "hoge hoge");
		string_node = string_node->next->content.string;
		CHECK_EQ(string_node->type, TOKTYPE_NON_EXPANDABLE);
		CHECK_EQ_STR(string_node->text, "$ABC");

		char **actual = expand_string_node(args_node->string_node->content.string);
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
	}

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

	TEST_SECTION("cmd_ast_cmd2cmdinvo 環境変数");
	{
		/* 準備 */
		setenv("ABC", "abc", 1);
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "echo $ABC \n");
		t_token	tok;

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
	}

	TEST_SECTION("cmd_ast_cmd2cmdinvo ダブルクオーテーションで囲まれた環境変数");
	{
		/* 準備 */
		setenv("ABC", "abc", 1);
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "echo \"$ABC\" \n");
		t_token	tok;

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
	}

	TEST_SECTION("cmd_ast_cmd2cmdinvo シングルクオーテーションで囲まれた環境変数");
	{
		/* 準備 */
		setenv("ABC", "abc", 1);
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "echo \'$ABC\' \n");
		t_token	tok;

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
	}


	TEST_SECTION("cmd_ast_cmd2cmdinvo スペースが含まれている環境変数");
	{
		/* 準備 */
		setenv("ABC", "abc def", 1);
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "echo $ABC \n");
		t_token	tok;

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
	}

	TEST_SECTION("cmd_ast_cmd2cmdinvo 文字列と環境変数");
	{
		/* 準備 */
		setenv("ABC", "abc def", 1);
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "echo hoge$ABC \n");
		t_token	tok;

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_command(&buf, &tok);
        CHECK_EQ(node->type, ASTNODE_COMMAND);
		CHECK_EQ(node->content.command->arguments_node->type, ASTNODE_ARGUMENTS);
		t_parse_node_arguments *args_node = node->content.command->arguments_node->content.arguments;
		CHECK_EQ(args_node->string_node->content.string->type, TOKTYPE_EXPANDABLE);
		CHECK_EQ_STR(args_node->string_node->content.string->text, "echo");
		args_node = args_node->rest_node->content.arguments;
		CHECK_EQ(args_node->string_node->content.string->type, TOKTYPE_EXPANDABLE);
		CHECK_EQ_STR(args_node->string_node->content.string->text, "hoge$ABC");

		/* テスト */
		t_command_invocation *actual = cmd_ast_cmd2cmdinvo(node->content.command);
		t_command_invocation *expected = cmd_init_cmdinvo((const char **)ft_split("echo hogeabc def", ' '));
		CHECK(actual);
		check_cmdinvo(actual, expected);

		cmd_free_cmdinvo(actual);
		cmd_free_cmdinvo(expected);
		unsetenv("ABC");
	}


	TEST_SECTION("cmd_ast_cmd2cmdinvo スペースが含まれている環境変数とダブルクオーテーション");
	{
		/* 準備 */
		setenv("ABC", "abc def", 1);
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "echo $ABC\"ghi jkl\" \n");
		t_token	tok;

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
	}

	TEST_SECTION("cmd_ast_cmd2cmdinvo 環境変数が複数");
	{
		/* 準備 */
		setenv("ABC", "abc def", 1);
		setenv("DEF", "ABC DEF", 1);
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "echo $ABC$DEF \n");
		t_token	tok;

		lex_get_token(&buf, &tok);

		t_parse_ast *node = parse_command(&buf, &tok);
        CHECK_EQ(node->type, ASTNODE_COMMAND);
		CHECK_EQ(node->content.command->arguments_node->type, ASTNODE_ARGUMENTS);
		t_parse_node_arguments *args_node = node->content.command->arguments_node->content.arguments;
		CHECK_EQ(args_node->string_node->content.string->type, TOKTYPE_EXPANDABLE);
		CHECK_EQ_STR(args_node->string_node->content.string->text, "echo");
		args_node = args_node->rest_node->content.arguments;
		CHECK_EQ(args_node->string_node->content.string->type, TOKTYPE_EXPANDABLE);
		CHECK_EQ_STR(args_node->string_node->content.string->text, "$ABC$DEF");

		/* テスト */
		t_command_invocation *actual = cmd_ast_cmd2cmdinvo(node->content.command);
		t_command_invocation *expected = cmd_init_cmdinvo((const char **)ft_split("echo abc defABC DEF", ' '));
		CHECK(actual);
		check_cmdinvo(actual, expected);

		cmd_free_cmdinvo(actual);
		cmd_free_cmdinvo(expected);
		unsetenv("ABC");
		unsetenv("DEF");
	}

	TEST_SECTION("cmd_ast_cmd2cmdinvo 存在しない環境変数");
	{
		/* 準備 */
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "echo $DONTEXISTS \n");
		t_token	tok;

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
