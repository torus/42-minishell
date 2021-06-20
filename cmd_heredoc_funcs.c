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
	return 0;
}

static void	heredoc_sigint_sighandler(int sig)
{
	g_shell.heredoc_interruption = 1;
	set_status(128 + sig);
}

void	cmd_set_heredoc_sighandlers()
{
	g_shell.heredoc_interruption = 0;
	if (signal(SIGQUIT, SIG_IGN) == SIG_ERR
		|| signal(SIGINT, heredoc_sigint_sighandler) == SIG_ERR)
	{
		printf("signal() failed\n");
		exit(1);
	}
}
