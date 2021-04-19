#include <stdlib.h>
#include "parse.h"
#include "parse2.h"

/*
**command_line ::=
**		"\n"
**	  | sequential_commands delimiter "\n"
**	  | sequential_commands "\n"
*/

t_parse_result	parse_command_line(
	t_parse_buffer *buf, t_parse_ast **node, t_token *tok)
{
	t_parse_ast		*cmdline_node;
	t_parse_ast		*seqcmd_node;
	t_parse_ast		*delim_node;
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
	t_parse_buffer *buf, t_parse_ast **node, t_token *tok)
{
	t_parse_ast		*delim_node;
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

t_parse_result	parse_sequential_commands(
	t_parse_buffer *buf, t_parse_ast **node, t_token *tok)
{
	t_parse_ast		*seq_node;
	t_parse_ast		*pipcmd_node;
	t_parse_ast		*delimiter_node;
	t_parse_ast		*rest_node;
	t_parse_node_seqcmds	*content;

	parse_skip_spaces(buf, tok);
	if (parse_piped_commands(buf, &pipcmd_node, tok) != PARSE_OK)
		return (PARSE_KO);
	content = malloc(sizeof(t_parse_node_seqcmds));
	seq_node = parse_new_ast_node(ASTNODE_SEQ_COMMANDS, content);
	content->pipcmd_node = pipcmd_node;
	content->delimiter_node = NULL;
	content->rest_node = NULL;
	if (parse_delimiter(buf, &delimiter_node, tok) == PARSE_OK)
	{
		lex_get_token(buf, tok);
		parse_skip_spaces(buf, tok);
		if (parse_sequential_commands(buf, &rest_node, tok) != PARSE_OK)
			return (PARSE_KO);
		content->delimiter_node = delimiter_node;
		content->rest_node = rest_node;
	}
	*node = seq_node;
	return (PARSE_OK);
}
