#include "builtin.h"
#include "path.h"

int	builtin_pwd(char **argv)
{
	(void)argv;
	if (!g_cwd)
		g_cwd = getcwd(NULL, 0);
	if (g_cwd)
		ft_putendl_fd(g_cwd, STDOUT_FILENO);
	return (0);
}
