#include <stdio.h>
#include "parse.h"

void	lex_get_lt_gt(t_parse_buffer *buf, t_token *result, int ch)
{
	if (ch == '<')
	{
		ch = lex_getc(buf);
		if (ch == '<')
			result->type = TOKTYPE_HEREDOCUMENT;
		else
		{
			lex_ungetc(buf);
			result->type = TOKTYPE_INPUT_REDIRECTION;
		}
		result->length = 0;
	}
	else
	{
		ch = lex_getc(buf);
		if (ch == '>')
			result->type = TOKTYPE_OUTPUT_APPENDING;
		else
		{
			result->type = TOKTYPE_OUTPUT_REDIRECTION;
			if (ch != EOF)
				lex_ungetc(buf);
		}
		result->length = 1;
	}
}
