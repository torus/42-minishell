#include "minishell.h"

static void sigint_sighandler(int sig)
{
	ft_putstr_fd("\n\r"PROMPT, STDOUT_FILENO);
	set_status(128 + sig);
}

void	setup_signal_handlers(void)
{
	if (signal(SIGQUIT, SIG_IGN) == SIG_ERR)
		exit(1);
	if (signal(SIGINT, sigint_sighandler) == SIG_ERR)
		exit(1);
}

