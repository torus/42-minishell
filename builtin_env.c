#include <unistd.h>
#include "builtin.h"
#include "libft/libft.h"
#include "env.h"
#include "minishell.h"

int	builtin_env(char **argv)
{
	int			i;
	char		**envs;

	(void)argv;
	i = 0;
	envs = vars2environ(g_shell.vars);
	while (envs[i])
	{
		if (ft_strchr(envs[i], '='))
			ft_putendl_fd(envs[i++], STDOUT_FILENO);
	}
	return (0);
}
