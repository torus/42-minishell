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
	t_var		*tmp_var;

	tmp_var = g_shell.vars;
	while (tmp_var)
	{
		if (!tmp_var->is_shell_var)
		{
			write(STDOUT_FILENO, "declare -x ", 11);
			write(STDOUT_FILENO, tmp_var->key, ft_strlen(tmp_var->key));
			if (tmp_var->value)
			{
				write(STDOUT_FILENO, "=", 1);
				write(STDOUT_FILENO, "\"", 1);
				ft_putstr_fd((char *)tmp_var->value, STDOUT_FILENO);
				write(STDOUT_FILENO, "\"", 1);
			}
			write(STDOUT_FILENO, "\n", 1);
		}
		tmp_var = tmp_var->next;
	}
	return (0);
}
