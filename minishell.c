#include "libft/libft.h"
#include "minishell.h"
#include "parse.h"

void	die(void)
{
	int	*nullpo;

	nullpo = NULL;
	nullpo[0xdead] = 1;
}

void	init_buffer(t_parse_buffer *buf)
{
	buf->cur_pos = 0;
	buf->size = 0;
}

void	invoke_sequential_commands(t_parse_ast *seqcmd)
{
	int						status;
	t_command_invocation	*inv;

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
}

int	main(int argc, char **argv)
{
	t_parse_ast				*cmdline;
	t_parse_buffer			buf;
	t_token					tok;
	t_parse_ast				*seqcmd;

	init_buffer(&buf);
	printf("Welcome Minishell\n");
	while (1)
	{
		printf("minish > ");
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
