#include "minishell.h"
#include "builtin.h"
#include "path.h"

int	builtin_pwd(char **argv)
{
	(void)argv;
	if (!g_shell.cwd)
		g_shell.cwd = getcwd(NULL, 0);
	if (g_shell.cwd)
		ft_putendl_fd(g_shell.cwd, STDOUT_FILENO);
	return (0);
}
