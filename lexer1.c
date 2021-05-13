#include <stdio.h>
#include <unistd.h>

#include "libft/libft.h"
#include "parse.h"

int	lex_getc(t_parse_buffer *buf)
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

void	lex_ungetc(t_parse_buffer *buf)
{
	buf->cur_pos--;
}

int	lex_get_symbols(t_parse_buffer *buf, t_token *result, int ch)
{
	if (ch == ';')
		result->type = TOKTYPE_SEMICOLON;
	else if (ch == '|')
		result->type = TOKTYPE_PIPE;
	else if (ch == '<')
	{
		result->type = TOKTYPE_INPUT_REDIRECTION;
		result->length = 0;
	}
	else if (ch == '>')
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
	else
		return (0);
	return (1);
}

int	lex_get_quoted(t_parse_buffer *buf, t_token *result, int ch)
{
	if (buf->lex_stat != LEXSTAT_NORMAL)
		parse_die();
	if (ch == '"')
	{
		buf->lex_stat = LEXSTAT_DOUBLE_QUOTED;
		result->type = TOKTYPE_EXPANDABLE_QUOTED;
		return (lex_read_double_quoted(buf, result));
	}
	else if (ch == '\'')
	{
		buf->lex_stat = LEXSTAT_SINGLE_QUOTED;
		result->type = TOKTYPE_NON_EXPANDABLE;
		return (lex_read_single_quoted(buf, result));
	}
	return (0);
}

int	lex_get_token(t_parse_buffer *buf, t_token *result)
{
	int	ch;
	int	ret;

	ret = 0;
	ch = lex_getc(buf);
	if (buf->lex_stat == LEXSTAT_DOUBLE_QUOTED)
		ret = lex_read_double_quoted(buf, result);
	else if (buf->lex_stat == LEXSTAT_SINGLE_QUOTED)
		ret = lex_read_single_quoted(buf, result);
	else if (lex_get_spaces(buf, result, ch))
		ret = 1;
	else if (lex_get_symbols(buf, result, ch))
		ret = 1;
	else if (lex_get_quoted(buf, result, ch))
		ret = 1;
	else if (ch != EOF)
	{
		result->type = TOKTYPE_EXPANDABLE;
		lex_ungetc(buf);
		ret = lex_read_word(buf, result);
		if (ret)
			lex_check_redirection_with_fd(buf, result);
	}
	return (ret);
}
