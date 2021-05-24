#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include "libft/libft.h"
#include "builtin.h"
#include "env.h"
#include "minishell.h"

int	print_envs_with_declaration(void)
{
	extern char	**environ;
	int			i;
	char		**kvarr;
	char		*env_val;

	i = 0;
	while (environ[i])
	{
		kvarr = split_first_c(environ[i], '=');
		if (!kvarr)
			return (ERROR);
		env_val = get_env_val(kvarr[0]);
		free_ptrarr((void **)kvarr);
		if (!env_val)
			return (ERROR);
		write(STDOUT_FILENO, "declare -x ", ft_strlen("declare -x "));
		write(STDOUT_FILENO, environ[i],
			ft_strchr(environ[i], '=') - environ[i] + 1);
		write(STDOUT_FILENO, "\"", 1);
		ft_putstr_fd(env_val, STDOUT_FILENO);
		free(env_val);
		write(STDOUT_FILENO, "\"\n", 2);
		i++;
	}
	return (0);
}
