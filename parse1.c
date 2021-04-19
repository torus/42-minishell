#include <stdlib.h>
#include "parse.h"
#include "parse2.h"

/*
**command_line ::=
**		"\n"
**	  | sequential_commands delimiter "\n"
**	  | sequential_commands "\n"
*/

t_parse_ast	*parse_command_line(
	t_parse_buffer *buf, t_token *tok)
{
	t_parse_ast				*cmdline_node;
	t_parse_ast				*seqcmd_node;
	t_parse_ast				*delim_node;
	t_parse_node_cmdline	*content_node;

	seqcmd_node = parse_sequential_commands(buf, tok);
	if (!seqcmd_node)
		return (NULL);
	delim_node = parse_delimiter(buf, tok);
	parse_skip_spaces(buf, tok);
	if (tok->type != TOKTYPE_NEWLINE)
		return (NULL);
	content_node = malloc(sizeof(t_parse_node_cmdline));
	cmdline_node = parse_new_ast_node(ASTNODE_COMMAND_LINE, content_node);
	content_node->seqcmd_node = seqcmd_node;
	content_node->delimiter_node = delim_node;
	return (cmdline_node);
}

/*
**delimiter ::=
**		";"
**	  | (bonus) "&"
*/

t_parse_ast	*parse_delimiter(
	t_parse_buffer *buf, t_token *tok)
{
	t_parse_ast				*delim_node;
	t_parse_node_delimiter	*content_node;

	parse_skip_spaces(buf, tok);
	if (tok->type != TOKTYPE_SEMICOLON)
		return (NULL);
	content_node = malloc(sizeof(t_parse_node_delimiter));
	delim_node = parse_new_ast_node(ASTNODE_DELIMITER, content_node);
	content_node->type = TOKTYPE_SEMICOLON;
	return (delim_node);
}

/*
** FIXME: this rule doesn't handle the "piped_commands delimiter \n" case.
**sequential_commands ::=
**		piped_commands delimiter sequential_commands
**	  | piped_commands
**	  | (bonus) piped_commands "&&" sequential_commands
**	  | (bonus) piped_commands "||" sequential_commands
*/

t_parse_ast	*parse_sequential_commands(
	t_parse_buffer *buf, t_token *tok)
{
	t_parse_ast				*seq_node;
	t_parse_ast				*pipcmd_node;
	t_parse_node_seqcmds	*content;

	parse_skip_spaces(buf, tok);
	pipcmd_node = parse_piped_commands(buf, tok);
	if (!pipcmd_node)
		return (NULL);
	content = malloc(sizeof(t_parse_node_seqcmds));
	seq_node = parse_new_ast_node(ASTNODE_SEQ_COMMANDS, content);
	content->pipcmd_node = pipcmd_node;
	content->delimiter_node = parse_delimiter(buf, tok);
	content->rest_node = NULL;
	if (content->delimiter_node)
	{
		lex_get_token(buf, tok);
		parse_skip_spaces(buf, tok);
		content->rest_node = parse_sequential_commands(buf, tok);
		if (!content->rest_node)
			return (NULL);
	}
	return (seq_node);
}
