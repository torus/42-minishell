#include <stdlib.h>
#include "libft/libft.h"
#include "parse.h"

/*
**piped_commands ::=
**		command "|" piped_commands
**	  | command
*/

t_parse_ast	*parse_piped_commands(t_parse_buffer *buf, t_token *tok)
{
	t_parse_ast				*pip_node;
	t_parse_node_pipcmds	*content_node;
	t_parse_ast				*cmd_node;
	t_parse_ast				*rest_node;

	cmd_node = parse_command(buf, tok);
	if (!cmd_node)
		return (NULL);
	content_node = malloc(sizeof(t_parse_node_pipcmds));
	pip_node = parse_new_ast_node(ASTNODE_PIPED_COMMANDS, content_node);
	content_node->command_node = cmd_node;
	rest_node = NULL;
	parse_skip_spaces(buf, tok);
	if (tok->type == TOKTYPE_PIPE)
	{
		lex_get_token(buf, tok);
		parse_skip_spaces(buf, tok);
		rest_node = parse_piped_commands(buf, tok);
		if (!rest_node)
			return (NULL);
	}
	content_node->next = rest_node;
	return (pip_node);
}

/*
**command ::=
**		arguments
**	  | (bonus) "(" sequential_commands ")"
**	  | (bonus) "(" sequential_commands delimiter ")"
*/

t_parse_ast	*parse_command(t_parse_buffer *buf, t_token *tok)
{
	t_parse_ast				*cmd_node;
	t_parse_node_command	*content_node;
	t_parse_ast				*args_node;

	args_node = parse_arguments(buf, tok);
	if (!args_node)
		return (NULL);
	content_node = malloc(sizeof(t_parse_node_command));
	cmd_node = parse_new_ast_node(ASTNODE_COMMAND, content_node);
	content_node->arguments_node = args_node;
	return (cmd_node);
}

/*
**arguments ::=
**		redirection
**	  | redirection arguments
**	  | string
**	  | string arguments
*/

t_parse_ast	*parse_arguments(t_parse_buffer *buf, t_token *tok)
{
	t_parse_ast				*string_node;
	t_parse_ast				*redirection_node;
	t_parse_ast				*rest_node;
	t_parse_node_arguments	*content_node;
	t_parse_ast				*args_node;

	rest_node = NULL;
	parse_skip_spaces(buf, tok);
	redirection_node = parse_redirection(buf, tok);
	string_node = parse_string(buf, tok);
	if (!redirection_node && !string_node)
		return (NULL);
	rest_node = parse_arguments(buf, tok);
	content_node = malloc(sizeof(t_parse_node_arguments));
	args_node = parse_new_ast_node(ASTNODE_ARGUMENTS, content_node);
	content_node->string_node = string_node;
	content_node->redirection_node = redirection_node;
	content_node->rest_node = rest_node;
	return (args_node);
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
t_parse_ast	*parse_string(t_parse_buffer *buf, t_token *tok)
{
	t_parse_ast			*new_node;
	t_parse_node_string	*string;
	char				*text;
	t_token_type		type;

	type = tok->type;
	if (type != TOKTYPE_EXPANDABLE
		&& type != TOKTYPE_NON_EXPANDABLE
		&& type != TOKTYPE_EXPANDABLE_QUOTED)
		return (NULL);
	string = malloc(sizeof(t_parse_node_string));
	new_node = parse_new_ast_node(ASTNODE_STRING, string);
	text = malloc(tok->length + 1);
	if (!string || !text)
		parse_fatal_error();
	string->type = type;
	ft_memcpy(text, tok->text, tok->length);
	text[tok->length] = '\0';
	string->text = text;
	lex_get_token(buf, tok);
	string->next = parse_string(buf, tok);
	return (new_node);
}

/*
**redirection ::=
**		"<" string
**	  | ">" string
**	  | ">>" string
*/
t_parse_ast	*parse_redirection(
	t_parse_buffer *buf, t_token *tok)
{
	t_parse_ast					*new_node;
	t_parse_ast					*str_node;
	t_parse_node_redirection	*redirection;
	t_token_type				type;
	int							fd;

	type = tok->type;
	if (type != TOKTYPE_INPUT_REDIRECTION
		&& type != TOKTYPE_OUTPUT_REDIRECTION
		&& type != TOKTYPE_OUTPUT_APPENDING)
		return (NULL);
	fd = tok->length;
	lex_get_token(buf, tok);
	parse_skip_spaces(buf, tok);
	str_node = parse_string(buf, tok);
	if (str_node)
	{
		redirection = malloc(sizeof(t_parse_node_redirection));
		redirection->type = type;
		redirection->fd = fd;
		redirection->string_node = str_node;
		new_node = parse_new_ast_node(ASTNODE_REDIRECTION, redirection);
		return (new_node);
	}
	return (NULL);
}
