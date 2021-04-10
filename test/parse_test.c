#include "test.h"
#include "../parse.h"

void	init_buf_with_string(t_parse_buffer *buf, const char* str)
{
	buf->cur_pos = 0;
	strcpy(buf->buffer, str);
	buf->size = strlen(str);
}

int main()
{
	TEST_CHAPTER("レキサー");

	TEST_SECTION("token_getc");
	{
		t_parse_buffer buf;
		init_buf_with_string(&buf, "abcde");
		int x = token_getc(&buf);
		CHECK_EQ(x, 'a');
		x = token_getc(&buf);
		CHECK_EQ(x, 'b');
	}

	TEST_SECTION("token_read_word(buf, result)");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "cat ");
		t_token	tok;
		token_read_word(&buf, &tok);
		CHECK_EQ(tok.length, 3);
		CHECK(!strncmp(tok.text, "cat", 3));
	}

	TEST_SECTION("token_get_token クォートなしの場合");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "cat $ABC |wc> file.txt\n");
		t_token	tok;

		token_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_EXPANDABLE);
		CHECK_EQ(tok.length, 3);
		CHECK(!strncmp(tok.text, "cat", 3));

		token_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_SPACE);

		token_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_EXPANDABLE);
		CHECK_EQ(tok.length, 4);
		CHECK(!strncmp(tok.text, "$ABC", 4));

		token_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_SPACE);

		token_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_PIPE);

		token_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_EXPANDABLE);
		CHECK_EQ(tok.length, 2);
		CHECK(!strncmp(tok.text, "wc", 2));

		token_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_OUTPUT_REDIRECTION);
		token_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_SPACE);

		token_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_EXPANDABLE);
		CHECK_EQ(tok.length, 8);
		CHECK(!strncmp(tok.text, "file.txt", 8));

		token_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_NEWLINE);
	}

	TEST_SECTION("token_get_token クォートありの場合");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "cat\"$ABC\"'wc'");
		t_token	tok;

		token_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_EXPANDABLE);
		CHECK_EQ(tok.length, 3);
		CHECK(!strncmp(tok.text, "cat", 3));

		token_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_EXPANDABLE_QUOTED);
		CHECK_EQ(tok.length, 4);
		CHECK(!strncmp(tok.text, "$ABC", 4));

		token_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_NON_EXPANDABLE);
		CHECK_EQ(tok.length, 2);
		CHECK(!strncmp(tok.text, "wc", 2));
	}

	TEST_SECTION("token_get_token のこり");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "cat < - ; ");
		t_token	tok;

		token_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_EXPANDABLE);
		CHECK_EQ(tok.length, 3);
		CHECK(!strncmp(tok.text, "cat", 3));

		token_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_SPACE);

		token_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_INPUT_REDIRECTION);

		token_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_SPACE);

		token_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_EXPANDABLE);
		CHECK_EQ(tok.length, 1);
		CHECK(!strncmp(tok.text, "-", 1));

		token_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_SPACE);

		token_get_token(&buf, &tok);
		CHECK_EQ(tok.type, TOKTYPE_SEMICOLON);
	}

	TEST_CHAPTER("パーサ");

	TEST_SECTION("parse_string 単純な文字列");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "file\n");
		t_parse_ast_node	*node;
		t_token	tok;

		token_get_token(&buf, &tok);

		int ret = parse_string(&buf, &node, &tok);
		CHECK_EQ(ret, PARSE_OK);
		CHECK(node);
		CHECK_EQ(node->type, ASTNODE_STRING);
		CHECK_EQ(node->content.string->type, TOKTYPE_EXPANDABLE);
		CHECK_EQ_STR(node->content.string->text, "file");
		CHECK_EQ(node->content.string->next, NULL);
	}

	TEST_SECTION("parse_redirection");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "< file\n");
		t_parse_ast_node	*node = NULL;
		t_token	tok;

		token_get_token(&buf, &tok);

		int ret = parse_redirection(&buf, &node, &tok);
		CHECK_EQ(ret, PARSE_OK);
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
		t_parse_ast_node	*node = NULL;
		t_token	tok;

		token_get_token(&buf, &tok);

		int ret = parse_arguments(&buf, &node, &tok);
		CHECK_EQ(ret, PARSE_OK);
		CHECK(node);
		CHECK_EQ(node->type, ASTNODE_ARGUMENTS);
		t_parse_ast_node *str_node = node->content.arguments->string_node;
		CHECK(str_node);
		CHECK_EQ(str_node->type, ASTNODE_STRING);
		CHECK_EQ_STR(str_node->content.string->text, "abc");
	}

	TEST_SECTION("parse_arguments 2 個");
	{
		t_parse_buffer	buf;
		init_buf_with_string(&buf, "abc def \n");
		t_parse_ast_node	*node = NULL;
		t_token	tok;

		token_get_token(&buf, &tok);

		int ret = parse_arguments(&buf, &node, &tok);
		CHECK_EQ(ret, PARSE_OK);
		CHECK(node);
		CHECK_EQ(node->type, ASTNODE_ARGUMENTS);
		t_parse_ast_node *str_node = node->content.arguments->string_node;
		CHECK(str_node);
		CHECK_EQ(str_node->type, ASTNODE_STRING);
		CHECK_EQ_STR(str_node->content.string->text, "abc");

		t_parse_ast_node *rest_node = node->content.arguments->rest_node;

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
		t_parse_ast_node	*node = NULL;
		t_token	tok;

		token_get_token(&buf, &tok);

		int ret = parse_arguments(&buf, &node, &tok);
		CHECK_EQ(ret, PARSE_OK);
		CHECK(node);
		CHECK_EQ(node->type, ASTNODE_ARGUMENTS);

		t_parse_ast_node *red_node = node->content.arguments->redirection_node;
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
		t_parse_ast_node	*node = NULL;
		t_token	tok;

		token_get_token(&buf, &tok);

		int ret = parse_arguments(&buf, &node, &tok);
		CHECK_EQ(ret, PARSE_OK);
		CHECK(node);
		CHECK_EQ(node->type, ASTNODE_ARGUMENTS);

		t_parse_ast_node *str_node = node->content.arguments->string_node;
		CHECK(str_node);
		CHECK_EQ(str_node->type, ASTNODE_STRING);
		CHECK_EQ_STR(str_node->content.string->text, "file");

		node = node->content.arguments->rest_node;
		t_parse_ast_node *red_node = node->content.arguments->redirection_node;
		CHECK(red_node);
		CHECK_EQ(red_node->type, ASTNODE_REDIRECTION);
		CHECK_EQ(red_node->content.redirection->type, TOKTYPE_INPUT_REDIRECTION);
		CHECK_EQ_STR(red_node->content.redirection->string_node
					 ->content.string->text, "abc");
	}

	int fail_count = print_result();
	return (fail_count);
}
