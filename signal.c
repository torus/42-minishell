#include <signal.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "minishell.h"
#include "env.h"
#include "editor.h"

static void	sigint_sighandler(int sig)
{
	rl_on_new_line();
	write(STDOUT_FILENO, "\n", 1);
	rl_replace_line("", 0);
	rl_redisplay();
	g_shell.interrupted = 1;
	set_status(128 + sig);
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
 * - SIGINT: Show "^C" since echo is disabled on the terminal.
 */
void	set_shell_sighandlers(void)
{
	if (signal(SIGQUIT, SIG_IGN) == SIG_ERR
		|| signal(SIGINT, sigint_sighandler) == SIG_ERR)
	{
		printf("signal() failed\n");
		exit(1);
	}
}

static void	sighandler_during_execution(int sig)
{
	g_shell.signal_child_received = sig;
}

void	set_sighandlers_during_execution(void)
{
	g_shell.signal_child_received = 0;
	if (signal(SIGQUIT, sighandler_during_execution) == SIG_ERR
		|| signal(SIGINT, sighandler_during_execution) == SIG_ERR)
	{
		printf("signal() failed\n");
		exit(1);
	}
}
