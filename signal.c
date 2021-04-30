#include "minishell.h"

static void sigint_sighandler(int sig)
{
	printf("receive SIGINT\n");
}

int	setup_signal_handlers(void)
{
	if (signal(SIGQUIT, SIG_IGN) == SIG_ERR)
		return (-1);
	if (signal(SIGINT, sigint_sighandler) == SIG_ERR)
		return (-1);
	return (0);
}

