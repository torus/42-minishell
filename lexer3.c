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
	if (ch == '<' || ch == '>')
	{
		fd = ft_atoi(result->text);
		lex_get_symbols(buf, result, ch);
		result->length = fd;
	}
	else
		lex_ungetc(buf);
	return (1);
}
