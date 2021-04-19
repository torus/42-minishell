#include "libft/libft.h"
#include "minishell.h"
#include "parse.h"

void	init_buffer(t_parse_buffer *buf)
{
	buf->cur_pos = 0;
	buf->size = 0;
}

int	main(int argc, char **argv)
{
	t_parse_ast	*cmdline;
	t_parse_buffer		buf;
	t_token				tok;

	init_buffer(&buf);
	printf("Welcome Minishell\n");
	while (1)
	{
		printf("minish > ");
		fflush(stdout);
		cmdline = NULL;
		lex_get_token(&buf, &tok);
		if (parse_command_line(&buf, &cmdline, &tok) == PARSE_OK)
			printf("TODO: process the command line... %p\n", cmdline);
		else
			put_err_msg("Parse error.");
	}
}
