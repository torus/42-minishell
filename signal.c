#include "minishell.h"

static void sigint_sighandler(int sig)
{
	ft_putstr_fd("\n\bminish > ", STDOUT_FILENO);
	set_status(128 + sig);
}

int	setup_signal_handlers(void)
{
	if (signal(SIGQUIT, SIG_IGN) == SIG_ERR)
		return (-1);
	if (signal(SIGINT, sigint_sighandler) == SIG_ERR)
		return (-1);
	return (0);
}

