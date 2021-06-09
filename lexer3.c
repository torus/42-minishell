#include <stdio.h>
#include "libft/libft.h"
#include "parse.h"

int	lex_check_redirection_with_fd(t_parse_buffer *buf, t_token *result)
{
	int	i;
	int	ch;
	int	fd;

	i = 0;
	while (i < result->length)
	{
		if (!ft_isdigit(result->text[i]))
			return (0);
		i++;
	}
	result->text[i] = '\0';
	ch = lex_getc(buf);
	if ((ch == '<' || ch == '>') && !is_int_overflow(result->text, 1))
	{
		fd = ft_atoi(result->text);
		lex_get_symbols(buf, result, ch);
		result->length = fd;
	}
	else
		lex_ungetc(buf);
	return (1);
}

int	lex_escaped(t_parse_buffer *buf, t_token *result)
{
	char	ch;

	ch = lex_getc(buf);
	if (ch == '\\')
	{
		ch = lex_getc(buf);
		if (ch != '\n'
			&& (buf->lex_stat == LEXSTAT_NORMAL
				|| (buf->lex_stat == LEXSTAT_DOUBLE_QUOTED
					&& ft_strchr("$\"\'\\`", ch))))
		{
			result->text[0] = ch;
			result->length = 1;
			result->type = TOKTYPE_NON_EXPANDABLE;
			return (1);
		}
		lex_ungetc(buf);
		result->text[0] = '\\';
		result->length = 1;
		result->type = TOKTYPE_NON_EXPANDABLE;
		return (1);
	}
	lex_ungetc(buf);
	return (0);
}

int	lex_get_eof(t_token *result, int ch)
{
	if (ch == EOF)
	{
		result->type = TOKTYPE_EOF;
		return (1);
	}
	return (0);
}

int	lex_init_token(t_token *result)
{
	result->max_length = 1024;
	result->text = malloc(result->max_length);
	if (!result->text)
	{
		printf("malloc token buffer failed\n");
		exit(1);
	}
	result->length = 0;
	return (0);
}

int	lex_expand_text_buf(t_token *result)
{
	char	*old_text;

	result->max_length *= 2;
	old_text = result->text;
	result->text = malloc(result->max_length);
	if (!result->text)
	{
		printf("expand token buffer failed\n");
		exit(1);
	}
	ft_memcpy(result->text, old_text, result->length);
	free(old_text);
	return (0);
}
