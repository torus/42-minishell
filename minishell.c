#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "libft/libft.h"
#include "minishell.h"
#include "parse.h"
#include "editor.h"

t_shell	g_shell;

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
	buf->lex_stat = LEXSTAT_NORMAL;
	ft_strlcpy(buf->buffer, str, len + 1);
	buf->getc = NULL;
	buf->ungetc = NULL;
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
		cmd_free_cmdinvo(inv);
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
	lex_init_token(&tok);
	lex_get_token(&buf, &tok);
	cmdline = parse_command_line(&buf, &tok);
	free(tok.text);
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

void	execute_sqcmd(t_parse_ast *cmdline)
{
	t_parse_ast			*seqcmd;

	seqcmd = cmdline->content.command_line->seqcmd_node;
	set_sighandlers_during_execution();
	invoke_sequential_commands(seqcmd);
	if (g_shell.signal_child_received)
	{
		if (g_shell.signal_child_received == SIGQUIT)
			write(STDOUT_FILENO, "Quit (core dumped)", 18);
		write(STDOUT_FILENO, "\n", 1);
		set_status(128 + g_shell.signal_child_received);
	}
	set_shell_sighandlers();
}

int	readline_and_execute(void)
{
	char				*input_str;
	t_token				tok;
	t_parse_ast			*cmdline;
	t_parse_buffer		buf;

	set_shell_sighandlers();
	while ((input_str = readline(MINISHELL_PROMPT)))
	{
		if (*input_str)
			add_history(input_str);
		init_buffer_with_string(&buf, input_str);
		buf.size++;
		buf.buffer[ft_strlen(input_str)] = '\n';
		lex_init_token(&tok);
		lex_get_token(&buf, &tok);
		cmdline = parse_command_line(&buf, &tok);
		free(tok.text);
		if (!cmdline)
		{
			put_err_msg("Parse error.");
			set_status(1);
		}
		else
			execute_sqcmd(cmdline);
		parse_free_all_ast();
		free(input_str);
	}
	write(1, "exit\n", 5);
	return (0);
}

int	main(int argc, char **argv)
{
	initialize_shell();
	if (argc == 3 && ft_strncmp(argv[1], "-c", 3) == 0)
		return (do_command(argv[2]));
	readline_and_execute();
}
