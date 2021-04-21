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

void check_strarr(const char **actual, const char **expected)
{
	int i = 0 ;
	while (actual[i] && expected[i])
	{
		CHECK_EQ_STR(actual[i], expected[i]);
		i++;
	}
	CHECK(!(actual || expected));  // 両方ともNULLだよね?
}

void check_cmdinvo(t_command_invocation *actual, t_command_invocation *expected)
{
	while (actual && expected)
	{
		if (actual->input_file_path || expected->input_file_path)
			CHECK_EQ_STR(actual->input_file_path, expected->input_file_path);
		if (actual->output_file_path || expected->output_file_path)
			CHECK_EQ_STR(actual->output_file_path, expected->output_file_path);
		check_strarr(actual->exec_and_args, expected->exec_and_args);
		actual = actual->piped_command;
		expected = expected->piped_command;
	}
	CHECK(!(actual || expected));  // 両方ともNULLだよね?
}

int main()
{
	TEST_SECTION("シンプルな文字列");
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
	}
}
