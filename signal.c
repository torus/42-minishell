#include "minishell.h"

int	setup_signal_handlers(void)
{
	if (signal(SIGQUIT, SIG_IGN) == SIG_ERR)
		return (-1);
	return (0);
}

