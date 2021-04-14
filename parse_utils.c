#include <stdlib.h>
#include "parse.h"

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
 * TODO: replace with proper error handling
 */
void	parse_fatal_error(void)
{
	exit(1);
}
