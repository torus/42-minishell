#include <signal.h>
#include <stdio.h>
#include "minishell.h"
#include "env.h"

static void	sigint_sighandler(int sig)
{
	ft_putstr_fd("\n\r"PROMPT, STDOUT_FILENO);
	set_status(128 + sig);
}

static void	sigquit_sighandler(int sig)
{
	(void)sig;
	ft_putstr_fd("\b\b  \b\b", STDOUT_FILENO);
}

void	set_sighandlers(t_sighandler sighandler)
{
	if (signal(SIGQUIT, sighandler) == SIG_ERR
		|| signal(SIGINT, sighandler) == SIG_ERR)
	{
		printf("signal() failed\n");
		exit(1);
	}
}

/*
 * Set signal handlers in shell process.
 *
 * This function set these signal handlers.
 * - SIGQUIT: Ignore signal. Do nothing.
 * - SIGINT: Line break and show prompt.
 */
void	set_shell_sighandlers(void)
{
	if (signal(SIGQUIT, sigquit_sighandler) == SIG_ERR
		|| signal(SIGINT, sigint_sighandler) == SIG_ERR)
	{
		printf("signal() failed\n");
		exit(1);
	}
}
