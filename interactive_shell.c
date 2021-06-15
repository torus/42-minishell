#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "minishell.h"

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

int	interactive_shell(void)
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
