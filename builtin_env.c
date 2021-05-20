#include <unistd.h>
#include "builtin.h"
#include "libft/libft.h"

int	builtin_env(char **argv)
{
	int			i;
	extern char	**environ;

	(void)argv;
	i = 0;
	while (environ[i])
	{
		if (ft_strchr(environ[i], '='))
			ft_putendl_fd(environ[i++], STDOUT_FILENO);
	}
	return (0);
}
