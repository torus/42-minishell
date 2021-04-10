#include <stdio.h>
#include <unistd.h>

#include "parse.h"

int	token_getc(t_parse_buffer *buf)
{
	if (buf->cur_pos == buf->size)
	{
		buf->size = read(STDIN_FILENO, buf->buffer, sizeof(buf->buffer));
		if (buf->size == 0)
			return (EOF);
		buf->cur_pos = 0;
	}
	return (buf->buffer[buf->cur_pos++]);
}

void	token_ungetc(t_parse_buffer *buf)
{
	buf->cur_pos--;
}

int	token_get_symbols(t_parse_buffer *buf, t_token *result, int ch)
{
	if (ch == ';')
		result->type = TOKTYPE_SEMICOLON;
	else if (ch == '|')
		result->type = TOKTYPE_PIPE;
	else if (ch == '<')
		result->type = TOKTYPE_INPUT_REDIRECTION;
	else if (ch == '>')
	{
		ch = token_getc(buf);
		if (ch == '>')
			result->type = TOKTYPE_OUTPUT_APPENDING;
		else
		{
			result->type = TOKTYPE_OUTPUT_REDIRECTION;
			if (ch != EOF)
				token_ungetc(buf);
		}
	}
	else
		return (0);
	return (1);
}

int	token_get_quoted(t_parse_buffer *buf, t_token *result, int ch)
{
	if (ch == '"')
	{
		result->type = TOKTYPE_EXPANDABLE_QUOTED;
		return (token_read_double_quoted(buf, result));
	}
	else if (ch == '\'')
	{
		result->type = TOKTYPE_NON_EXPANDABLE;
		return (token_read_single_quoted(buf, result));
	}
	return (0);
}

int	token_get_token(t_parse_buffer *buf, t_token *result)
{
	int				ch;

	ch = token_getc(buf);
	if (token_get_spaces(buf, result, ch))
		return (1);
	else if (token_get_symbols(buf, result, ch))
		return (1);
	else if (token_get_quoted(buf, result, ch))
		return (1);
	else if (ch != EOF)
	{
		result->type = TOKTYPE_EXPANDABLE;
		token_ungetc(buf);
		return (token_read_word(buf, result));
	}
	return (0);
}
