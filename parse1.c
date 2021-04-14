#include <stdlib.h>
#include "libft/libft.h"
#include "parse.h"
#include "parse2.h"

void	parse_skip_spaces(t_parse_buffer *buf, t_token *tok)
{
	while (1)
	{
		if (tok->type != TOKTYPE_SPACE)
			break ;
		token_get_token(buf, tok);
	}
}

t_parse_ast_node	*parse_new_ast_node(t_parse_ast_type type, void *content)
{
	t_parse_ast_node	*dest;

	if (!(type > ASTNODE_NONE && type < ASTNODE_INVALID))
		parse_fatal_error();
	dest = malloc(sizeof(t_parse_ast_node));
	if (!dest || !content)
		parse_fatal_error();
	dest->type = type;
	dest->content.void_ptr = content;
	return (dest);
}

/*
**command_line ::=
**		"\n"
**	  | sequential_commands delimiter "\n"
**	  | sequential_commands "\n"
*/

t_parse_result	parse_command_line(
	t_parse_buffer *buf, t_parse_ast_node **node, t_token *tok)
{
	t_parse_ast_node		*cmdline_node;
	t_parse_ast_node		*seqcmd_node;
	t_parse_ast_node		*delim_node;
	t_parse_node_cmdline	*content_node;

    if (parse_sequential_commands(buf, &seqcmd_node, tok) != PARSE_OK)
        return (PARSE_KO);
    delim_node = NULL;
    parse_delimiter(buf, &delim_node, tok);
    parse_skip_spaces(buf, tok);
    if (tok->type != TOKTYPE_NEWLINE)
        return (PARSE_KO);
    content_node = malloc(sizeof(t_parse_node_cmdline));
    cmdline_node = parse_new_ast_node(ASTNODE_COMMAND_LINE, content_node);
    content_node->seqcmd_node = seqcmd_node;
    content_node->delimiter_node = delim_node;
    *node = cmdline_node;
    return (PARSE_OK);
}

/*
**delimiter ::=
**		";"
**	  | (bonus) "&"
*/

t_parse_result	parse_delimiter(
	t_parse_buffer *buf, t_parse_ast_node **node, t_token *tok)
{
	t_parse_ast_node		*delim_node;
	t_parse_node_delimiter	*content_node;

	parse_skip_spaces(buf, tok);
	if (tok->type != TOKTYPE_SEMICOLON)
		return (PARSE_KO);
	content_node = malloc(sizeof(t_parse_node_delimiter));
	delim_node = parse_new_ast_node(ASTNODE_DELIMITER, content_node);
	content_node->type = TOKTYPE_SEMICOLON;
	*node = delim_node;
	return (PARSE_OK);
}

/*
** FIXME: this rule doesn't handle the "piped_commands delimiter \n" case.
**sequential_commands ::=
**		piped_commands delimiter sequential_commands
**	  | piped_commands
**	  | (bonus) piped_commands "&&" sequential_commands
**	  | (bonus) piped_commands "||" sequential_commands
*/

#include <stdio.h>

t_parse_result	parse_sequential_commands(
	t_parse_buffer *buf, t_parse_ast_node **node, t_token *tok)
{
	t_parse_ast_node		*seq_node;
	t_parse_ast_node		*pipcmd_node;
	t_parse_ast_node		*delimiter_node;
	t_parse_ast_node		*rest_node;
    t_parse_node_seqcmds	*content;

    parse_skip_spaces(buf, tok);
	if (parse_piped_commands(buf, &pipcmd_node, tok) != PARSE_OK)
		return (PARSE_KO);
    content = malloc(sizeof(t_parse_node_seqcmds));
    seq_node = parse_new_ast_node(ASTNODE_SEQ_COMMANDS, content);
    content->pipcmd_node = pipcmd_node;
    content->delimiter_node = NULL;
    content->rest_node = NULL;
    parse_skip_spaces(buf, tok);
    if (parse_delimiter(buf, &delimiter_node, tok) == PARSE_OK)
    {
        token_get_token(buf, tok);
        parse_skip_spaces(buf, tok);
        if (parse_sequential_commands(buf, &rest_node, tok) != PARSE_OK)
            return (PARSE_KO);
        content->delimiter_node = delimiter_node;
        content->rest_node = rest_node;
    }

    *node = seq_node;
	return (PARSE_OK);
}

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
		token_get_token(buf, tok);
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
	if (!(parse_redirection(buf, &redirection_node, tok) == PARSE_OK)
		&& !(parse_string(buf, &string_node, tok) == PARSE_OK))
		return (PARSE_KO);
	token_get_token(buf, tok);
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
	token_get_token(buf, tok);
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
	token_get_token(buf, tok);
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

/*
 * TODO: replace with proper error handling
 */
void	parse_fatal_error(void)
{
	exit(1);
}
