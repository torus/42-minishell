#include "minishell.h"

static void	sigint_sighandler(int sig)
{
	ft_putstr_fd("\n\r"PROMPT, STDOUT_FILENO);
	set_status(128 + sig);
}

void	set_sighandlers(__sighandler_t sighandler)
{
	if (signal(SIGQUIT, sighandler) == SIG_ERR)
		exit(1);
	if (signal(SIGINT, sighandler) == SIG_ERR)
		exit(1);
}

/*
 * シェルでのシグナルハンドラーを設定する
 *
 * 具体的には以下のようなハンドラーを設定する
 * - SIGQUIT: 無視
 * - SIGINT: 改行してプロンプトを表示
 */
void	set_shell_sighandlers(void)
{
	if (signal(SIGQUIT, SIG_IGN) == SIG_ERR)
		exit(1);
	if (signal(SIGINT, sigint_sighandler) == SIG_ERR)
		exit(1);
}
