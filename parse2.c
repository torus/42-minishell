#include <stdlib.h>
#include "libft/libft.h"
#include "parse.h"
#include "parse2.h"

/*
**piped_commands ::=
**		command "|" piped_commands
**	  | command
*/

t_parse_result	parse_piped_commands(
	t_parse_buffer *buf, t_parse_ast_node **node, t_token *tok)
{
	t_parse_ast_node		*pip_node;
	t_parse_node_pipcmds	*content_node;
	t_parse_ast_node		*cmd_node;
	t_parse_ast_node		*rest_node;

	if (parse_command(buf, &cmd_node, tok) != PARSE_OK)
		return (PARSE_KO);
	content_node = malloc(sizeof(t_parse_node_pipcmds));
	pip_node = parse_new_ast_node(ASTNODE_PIPED_COMMANDS, content_node);
	content_node->command_node = cmd_node;
	rest_node = NULL;
	parse_skip_spaces(buf, tok);
	if (tok->type == TOKTYPE_PIPE)
	{
		lex_get_token(buf, tok);
		parse_skip_spaces(buf, tok);
		if (parse_piped_commands(buf, &rest_node, tok) != PARSE_OK)
			return (PARSE_KO);
	}
	content_node->next = rest_node;
	*node = pip_node;
	return (PARSE_OK);
}

/*
**command ::=
**		arguments
**	  | (bonus) "(" sequential_commands ")"
**	  | (bonus) "(" sequential_commands delimiter ")"
*/

t_parse_result	parse_command(
	t_parse_buffer *buf, t_parse_ast_node **node, t_token *tok)
{
	t_parse_ast_node		*cmd_node;
	t_parse_node_command	*content_node;
	t_parse_ast_node		*args_node;

	if (parse_arguments(buf, &args_node, tok) != PARSE_OK)
		return (PARSE_KO);
	content_node = malloc(sizeof(t_parse_node_command));
	cmd_node = parse_new_ast_node(ASTNODE_COMMAND, content_node);
	content_node->arguments_node = args_node;
	*node = cmd_node;
	return (PARSE_OK);
}

/*
**arguments ::=
**		redirection
**	  | redirection arguments
**	  | string
**	  | string arguments
*/

t_parse_result	parse_arguments(
	t_parse_buffer *buf, t_parse_ast_node **node, t_token *tok)
{
	t_parse_ast_node		*string_node;
	t_parse_ast_node		*redirection_node;
	t_parse_ast_node		*rest_node;
	t_parse_node_arguments	*content_node;
	t_parse_ast_node		*args_node;

	string_node = NULL;
	redirection_node = NULL;
	rest_node = NULL;
	parse_skip_spaces(buf, tok);
	if (!(parse_redirection(buf, &redirection_node, tok) == PARSE_OK)
		&& !(parse_string(buf, &string_node, tok) == PARSE_OK))
		return (PARSE_KO);
	parse_arguments(buf, &rest_node, tok);
	content_node = malloc(sizeof(t_parse_node_arguments));
	content_node->string_node = string_node;
	content_node->redirection_node = redirection_node;
	content_node->rest_node = rest_node;
	args_node = parse_new_ast_node(ASTNODE_ARGUMENTS, content_node);
	*node = args_node;
	return (PARSE_OK);
}

/*
**string ::=
**		expandable <no_space> string
**	  | expandable
**	  | not_expandable <no_space> string
**	  | not_expandable
**	  | expandable_quoted <no_space> string
**	  | expandable_quoted
*/
t_parse_result	parse_string(
	t_parse_buffer *buf, t_parse_ast_node **node, t_token *tok)
{
	t_parse_ast_node	*new_node;
	t_parse_node_string	*string;
	char				*text;

	if (tok->type != TOKTYPE_EXPANDABLE)
		return (PARSE_KO);
	string = NULL;
	string = malloc(sizeof(t_parse_node_string));
	new_node = parse_new_ast_node(ASTNODE_STRING, string);
	text = malloc(tok->length + 1);
	if (!string || !text)
		parse_fatal_error();
	string->type = tok->type;
	ft_memcpy(text, tok->text, tok->length);
	text[tok->length] = '\0';
	string->text = text;
	lex_get_token(buf, tok);
	parse_string(buf, &string->next, tok);
	*node = new_node;
	return (PARSE_OK);
}

/*
**redirection ::=
**		"<" string
**	  | ">" string
**	  | ">>" string
*/
t_parse_result	parse_redirection(
	t_parse_buffer *buf, t_parse_ast_node **node, t_token *tok)
{
	t_parse_ast_node			*new_node;
	t_parse_ast_node			*str_node;
	t_parse_node_redirection	*redirection;

	if (tok->type != TOKTYPE_INPUT_REDIRECTION)
		return (PARSE_KO);
	lex_get_token(buf, tok);
	parse_skip_spaces(buf, tok);
	if (parse_string(buf, &str_node, tok) == PARSE_OK)
	{
		redirection = malloc(sizeof(t_parse_node_redirection));
		redirection->type = TOKTYPE_INPUT_REDIRECTION;
		redirection->string_node = str_node;
		new_node = parse_new_ast_node(ASTNODE_REDIRECTION, redirection);
		*node = new_node;
		return (PARSE_OK);
	}
	return (PARSE_KO);
}
