#include <stdio.h>
#include "parse.h"

int	parse_is_special_char(char ch)
{
	if (ch == ' ' || ch == '\t' || ch == '\n' || ch == ';'
		|| ch == '|' || ch == '>' || ch == '<' || ch == '"' || ch == '\'')
		return (1);
	return (0);
}

int	parse_read_word(t_parse_buffer *buf, t_parse_token *result)
{
	int	pos;
	int	ch;

	pos = 0;
	while (1)
	{
		ch = parse_getc(buf);
		if (ch == EOF)
			break ;
		if (parse_is_special_char(ch))
		{
			parse_ungetc(buf);
			break ;
		}
		result->text[pos++] = ch;
	}
	result->length = pos;
	return (1);
}

int	parse_read_double_quoted(t_parse_buffer *buf, t_parse_token *result)
{
	int	pos;
	int	ch;

	pos = 0;
	while (1)
	{
		ch = parse_getc(buf);
		if (ch == '"' || ch == '\n' || ch == EOF)
			break ;
		result->text[pos++] = ch;
	}
	result->length = pos;
	return (1);
}

int	parse_read_single_quoted(t_parse_buffer *buf, t_parse_token *result)
{
	int	pos;
	int	ch;

	pos = 0;
	while (1)
	{
		ch = parse_getc(buf);
		if (ch == '\'' || ch == '\n' || ch == EOF)
			break ;
		result->text[pos++] = ch;
	}
	result->length = pos;
	return (1);
}

int	parse_get_spaces(t_parse_buffer *buf, t_parse_token *result, int ch)
{
	if (ch == ' ' || ch == '\t')
	{
		while (1)
		{
			ch = parse_getc(buf);
			if (ch != ' ' && ch != '\t')
			{
				if (ch != EOF)
					parse_ungetc(buf);
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
