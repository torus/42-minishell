#include <stdio.h>
#include "parse.h"

int	token_is_special_char(char ch)
{
	if (ch == ' ' || ch == '\t' || ch == '\n' || ch == ';'
		|| ch == '|' || ch == '>' || ch == '<' || ch == '"' || ch == '\'')
		return (1);
	return (0);
}

int	token_read_word(t_parse_buffer *buf, t_token *result)
{
	int	pos;
	int	ch;

	pos = 0;
	while (1)
	{
		ch = token_getc(buf);
		if (ch == EOF)
			break ;
		if (token_is_special_char(ch))
		{
			token_ungetc(buf);
			break ;
		}
		result->text[pos++] = ch;
	}
	result->length = pos;
	return (1);
}

int	token_read_double_quoted(t_parse_buffer *buf, t_token *result)
{
	int	pos;
	int	ch;

	pos = 0;
	while (1)
	{
		ch = token_getc(buf);
		if (ch == '"' || ch == '\n' || ch == EOF)
			break ;
		result->text[pos++] = ch;
	}
	result->length = pos;
	return (1);
}

int	token_read_single_quoted(t_parse_buffer *buf, t_token *result)
{
	int	pos;
	int	ch;

	pos = 0;
	while (1)
	{
		ch = token_getc(buf);
		if (ch == '\'' || ch == '\n' || ch == EOF)
			break ;
		result->text[pos++] = ch;
	}
	result->length = pos;
	return (1);
}

int	token_get_spaces(t_parse_buffer *buf, t_token *result, int ch)
{
	if (ch == ' ' || ch == '\t')
	{
		while (1)
		{
			ch = token_getc(buf);
			if (ch != ' ' && ch != '\t')
			{
				if (ch != EOF)
					token_ungetc(buf);
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
