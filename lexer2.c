#include <stdio.h>
#include "parse.h"

int	lex_is_special_char(char ch)
{
	if (ch == ' ' || ch == '\t' || ch == '\n' || ch == ';'
		|| ch == '|' || ch == '>' || ch == '<' || ch == '"' || ch == '\'')
		return (1);
	return (0);
}

int	lex_read_word(t_parse_buffer *buf, t_token *result)
{
	int	pos;
	int	ch;

	if (lex_escaped(buf, result))
		return (1);
	pos = 0;
	while (1)
	{
		ch = lex_getc(buf);
		if (ch == EOF)
			break ;
		if (ch == '\\' || lex_is_special_char(ch) || (ch == '$' && pos > 0))
		{
			lex_ungetc(buf);
			break ;
		}
		result->text[pos++] = ch;
		if (pos == PARSE_BUFFER_SIZE)
			break;
	}
	result->length = pos;
	return (1);
}

int	lex_read_double_quoted(t_parse_buffer *buf, t_token *result)
{
	int	pos;
	int	ch;

	if (lex_escaped(buf, result))
		return (1);
	pos = 0;
	while (1)
	{
		ch = lex_getc(buf);
		if (ch == '"' || ch == '\n' || ch == EOF)
			buf->lex_stat = LEXSTAT_NORMAL;
		if (ch == '\n' || ch == EOF)
			result->type = TOKTYPE_PARSE_ERROR;
		if (ch == '\\' || ch == '\n' || ch == EOF)
			lex_ungetc(buf);
		if (ch == '\\' || ch == '"' || ch == '\n' || ch == EOF)
			break ;
		result->text[pos++] = ch;
	}
	result->length = pos;
	return (1);
}

int	lex_read_single_quoted(t_parse_buffer *buf, t_token *result)
{
	int	pos;
	int	ch;

	pos = 0;
	while (1)
	{
		ch = lex_getc(buf);
		if (ch == '\'' || ch == '\n' || ch == EOF)
			buf->lex_stat = LEXSTAT_NORMAL;
		if (ch == '\n' || ch == EOF)
		{
			result->type = TOKTYPE_PARSE_ERROR;
			lex_ungetc(buf);
		}
		if (ch == '\'' || ch == '\n' || ch == EOF)
			break ;
		result->text[pos++] = ch;
	}
	result->length = pos;
	return (1);
}

int	lex_get_spaces(t_parse_buffer *buf, t_token *result, int ch)
{
	if (ch == ' ' || ch == '\t')
	{
		while (1)
		{
			ch = lex_getc(buf);
			if (ch != ' ' && ch != '\t')
			{
				if (ch != EOF)
					lex_ungetc(buf);
				result->type = TOKTYPE_SPACE;
				break ;
			}
		}
		return (1);
	}
	else if (ch == '\n')
	{
		result->type = TOKTYPE_NEWLINE;
		return (1);
	}
	return (0);
}
