#include <limits.h>
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
		result->length = (int)fd;
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
		result->text[0] = ch;
		result->length = 1;
		result->type = TOKTYPE_NON_EXPANDABLE;
		return (1);
	}
	lex_ungetc(buf);
	return (0);
}
