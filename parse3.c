#include <stdlib.h>
#include "libft/libft.h"
#include "parse.h"

/*
**command_line ::=
**		"\n"
**	  | sequencial_commands delimiter "\n"
**	  | sequencial_commands "\n"
**
**delimiter ::=
**		";"
**	  | (bonus) "&"
**
**sequencial_commands ::=
**		piped_commands delimiter sequencial_commands
**	  | piped_commands
**	  | (bonus) piped_commands "&&" sequencial_commands
**	  | (bonus) piped_commands "||" sequencial_commands
**
**piped_commands ::=
**		command "|" piped_commands
**	  | command
**
**command ::=
**		arguments
**	  | (bonus) "(" sequencial_commands ")"
**	  | (bonus) "(" sequencial_commands delimiter ")"
**
**arguments ::=
**		redirection
**	  | redirection arguments
**	  | string
**	  | string arguments
**
**string ::=
**		expandable <no_space> string
**	  | expandable
**	  | not_expandable <no_space> string
**	  | not_expandable
**	  | expandable_quoted <no_space> string
**	  | expandable_quoted
**
**redirection ::=
**		"<" string
**	  | ">" string
**	  | ">>" string
*/

int	parse_string(
	t_parse_buffer *buf, t_parse_ast_node **node, t_parse_token *tok)
{
	t_parse_ast_node	*new_node;
	t_parse_node_string	*string;
	t_parse_node_string	*next_string;
	char				*text;

	if (tok->type != TOKTYPE_EXPANDABLE)
		return (PARSE_KO);


	new_node = malloc(sizeof(t_parse_ast_node));
	if (!new_node)
		parse_fatal_error();
	new_node->type = ASTNODE_STRING;

	string = NULL;
	next_string = NULL;
	while (tok->type == TOKTYPE_EXPANDABLE)
	{
		next_string = malloc(sizeof(t_parse_node_string));
		text = malloc(tok->length + 1);
		if (!next_string || !text)
			parse_fatal_error();

		next_string->type = tok->type;

		ft_memcpy(text, tok->text, tok->length);
		text[tok->length] = '\0';
		next_string->text = text;
		if (string)
			string->next = next_string;
		string = next_string;

		parse_get_token(buf, tok);
	}
	next_string->next = NULL;
	new_node->content.string = next_string;
	*node = new_node;
	return (PARSE_OK);
}

int	parse_redirection(
	t_parse_buffer *buf, t_parse_ast_node **node, t_parse_token *tok)
{
	t_parse_ast_node	*new_node;
	t_parse_ast_node	*str_node;
	t_parse_node_redirection	*redirection;

	if (tok->type != TOKTYPE_INPUT_REDIRECTION)
		return (PARSE_KO);

	while (1)
	{
		parse_get_token(buf, tok);
		if (tok->type != TOKTYPE_SPACE)
			break ;
	}
	if (parse_string(buf, &str_node, tok) == PARSE_OK)
	{
		new_node = malloc(sizeof(t_parse_ast_node));
		redirection = malloc(sizeof(t_parse_node_redirection));
		if (!new_node || !redirection)
			parse_fatal_error();

		new_node->type = ASTNODE_REDIRECTION;
		redirection->type = TOKTYPE_INPUT_REDIRECTION;

		redirection->string_node = str_node;

		new_node->content.redirection = redirection;

		*node = new_node;
		return (PARSE_OK);
	}
	return (PARSE_KO);
}

void	parse_fatal_error(void)
{
	((int*)0)[0] = 1;
}
