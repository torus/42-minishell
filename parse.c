#include <stdio.h>
#include <unistd.h>

#include "parse.h"

int	parse_getc(t_parse_buffer *buf)
{
	if (buf->cur_pos == buf->size)
	{
		buf->size = read(STDIN_FILENO, buf->buffer, PARSE_BUFFER_SIZE);
		if (buf->size == 0)
			return (EOF);
		buf->cur_pos = 0;
	}
	return (buf->buffer[buf->cur_pos++]);
}

void	parse_ungetc(t_parse_buffer *buf)
{
	buf->cur_pos--;
}

int	parse_get_symbols(t_parse_buffer *buf, t_parse_token *result, int ch)
{
	if (ch == ';')
		result->type = TOKTYPE_SEMICOLON;
	else if (ch == '|')
		result->type = TOKTYPE_PIPE;
	else if (ch == '<')
		result->type = TOKTYPE_INPUT_REDIRECTION;
	else if (ch == '>')
	{
		ch = parse_getc(buf);
		if (ch == '>')
			result->type = TOKTYPE_OUTPUT_APPENDING;
		else
		{
			result->type = TOKTYPE_OUTPUT_REDIRECTION;
			if (ch != EOF)
				parse_ungetc(buf);
		}
	}
	else
		return (0);
	return (1);
}

int	parse_get_quoted(t_parse_buffer *buf, t_parse_token *result, int ch)
{
	if (ch == '"')
	{
		result->type = TOKTYPE_EXPANDABLE_QUOTED;
		return (parse_read_double_quoted(buf, result));
	}
	else if (ch == '\'')
	{
		result->type = TOKTYPE_NON_EXPANDABLE;
		return (parse_read_single_quoted(buf, result));
	}
	return (0);
}

int	parse_get_token(t_parse_buffer *buf, t_parse_token *result)
{
	int				ch;

	ch = parse_getc(buf);
	if (parse_get_spaces(buf, result, ch))
		return (1);
	else if (parse_get_symbols(buf, result, ch))
		return (1);
	else if (parse_get_quoted(buf, result, ch))
		return (1);
	else if (ch != EOF)
	{
		result->type = TOKTYPE_EXPANDABLE;
		parse_ungetc(buf);
		return (parse_read_word(buf, result));
	}
	return (0);
}
