#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include "libft/libft.h"
#include "builtin.h"
#include "env.h"
#include "minishell.h"
#include "utils.h"

static void	print_escaped_value(char *str)
{
	int	i;

	i = 0;
	while (str[i])
	{
		if (str[i] == '"' || str[i] == '$' || str[i] == '`' || str[i] == '\\')
		{
			ft_putchar_fd('\\', STDOUT_FILENO);
			ft_putchar_fd(str[i], STDOUT_FILENO);
		}
		else
			ft_putchar_fd(str[i], STDOUT_FILENO);
		i++;
	}
}

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
				print_escaped_value((char *)tmp_var->value);
				write(STDOUT_FILENO, "\"", 1);
			}
			write(STDOUT_FILENO, "\n", 1);
		}
		tmp_var = tmp_var->next;
	}
	return (0);
}
