#include "builtin.h"

int	builtin_env(char **argv)
{
	int			i;
	extern char	**environ;

	i = 0;
	while (environ[i])
		ft_putendl_fd(environ[i++], STDOUT_FILENO);
	return (0);
}
