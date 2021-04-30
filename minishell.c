#include "libft/libft.h"
#include "minishell.h"
#include "parse.h"

void	die(void)
{
	int	*nullpo;

	nullpo = NULL;
	nullpo[0xdead] = 1;
}

void	init_buffer_with_string(t_parse_buffer *buf, char *str)
{
	size_t	len;

	len = ft_strlen(str);
	buf->cur_pos = 0;
	buf->size = len;
	ft_strlcpy(buf->buffer, str, len + 1);
}

int	invoke_sequential_commands(t_parse_ast *seqcmd)
{
	t_command_invocation	*inv;
	int						status;

	while (seqcmd && seqcmd->content.sequential_commands->pipcmd_node)
	{
		if (seqcmd->type != ASTNODE_SEQ_COMMANDS)
			die();
		inv = cmd_ast_pipcmds2cmdinvo(
				seqcmd->content.sequential_commands->pipcmd_node
				->content.piped_commands);
		if (!inv)
			die();
		status = cmd_exec_commands(inv);
		seqcmd = seqcmd->content.sequential_commands->rest_node;
	}
	return (status);
}

int	do_command(char *cmdstr)
{
	t_parse_buffer	buf;
	t_parse_ast		*cmdline;
	t_token			tok;
	t_parse_ast		*seqcmd;
	size_t			len;

	len = ft_strlen(cmdstr);
	init_buffer_with_string(&buf, cmdstr);
	buf.size++;
	buf.buffer[len] = '\n';
	lex_get_token(&buf, &tok);
	cmdline = parse_command_line(&buf, &tok);
	if (cmdline)
	{
		seqcmd = cmdline->content.command_line->seqcmd_node;
		return (invoke_sequential_commands(seqcmd));
	}
	else
	{
		put_err_msg("Parse error.");
		return (1);
	}
}

int	main(int argc, char **argv)
{
	t_parse_ast				*cmdline;
	t_parse_buffer			buf;
	t_token					tok;
	t_parse_ast				*seqcmd;

	if (argc == 3
		&& argv[1][0] == '-' && argv[1][1] == 'c' && argv[1][2] == '\0')
		return (do_command(argv[2]));
	if (setup_signal_handlers())
		return (put_err_msg_and_ret("signal() failed"));
	init_buffer_with_string(&buf, "");
	printf("Welcome to Minishell\n");
	while (1)
	{
		printf(PROMPT);
		fflush(stdout);
		lex_get_token(&buf, &tok);
		cmdline = parse_command_line(&buf, &tok);
		if (cmdline)
		{
			seqcmd = cmdline->content.command_line->seqcmd_node;
			invoke_sequential_commands(seqcmd);
		}
		else
			put_err_msg("Parse error.");
	}
}
