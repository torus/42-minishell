#include <stdlib.h>
#include "parse.h"

void	parse_die(void)
{
	int	*nullpo;

	nullpo = NULL;
	nullpo[0] = 0xD1E;
}

void	parse_skip_spaces(t_parse_buffer *buf, t_token *tok)
{
	while (1)
	{
		if (tok->type != TOKTYPE_SPACE)
			break ;
		lex_get_token(buf, tok);
	}
}

void	parse_fatal_error(void)
{
	exit(1);
}
