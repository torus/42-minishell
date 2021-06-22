#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "execution.h"
#include "minishell.h"
#include "utils.h"

int	cmd_check_readline_has_finished(void)
{
	if (g_shell.heredoc_interruption)
		rl_done = 1;
	return (0);
}

static void	heredoc_sigint_sighandler(int sig)
{
	g_shell.heredoc_interruption = 1;
	set_status(128 + sig);
}

void	cmd_set_heredoc_sighandlers(void)
{
	g_shell.heredoc_interruption = 0;
	if (signal(SIGQUIT, SIG_IGN) == SIG_ERR
		|| signal(SIGINT, heredoc_sigint_sighandler) == SIG_ERR)
	{
		printf("signal() failed\n");
		exit(1);
	}
}

bool	cmd_is_heredoc_expandable(t_parse_node_redirection *redirection_node)
{
	bool				is_expandable_heredoc;
	t_parse_node_string	*str_node;

	str_node = redirection_node->string_node->content.string;
	is_expandable_heredoc = 1;
	while (str_node)
	{
		if (str_node->type == TOKTYPE_NON_EXPANDABLE)
			is_expandable_heredoc = 0;
		if (str_node->next)
			str_node = str_node->next->content.string;
		else
			str_node = NULL;
	}
	return (is_expandable_heredoc);
}
