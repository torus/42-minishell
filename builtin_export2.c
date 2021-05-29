#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include "libft/libft.h"
#include "builtin.h"
#include "env.h"
#include "minishell.h"
#include "utils.h"


static char    *escape_string(char *str)
{
	char	*result;
	char	*tmp;
	int		i;
	
	i = 0;
	result = ft_strdup("");
	while (str[i])
	{
		if (str[i] == '"' || str[i] == '$' || str[i] == '`' || str[i] == '\\')
		{
			tmp = ft_substr(str, 0, i);
			result = strjoin_and_free_both(result, tmp);
			result = strjoin_and_free_first(result, "\\");
			tmp = ft_substr(str + i, 0, 1);
			result = strjoin_and_free_both(result, tmp);
			str += i + 1;
			i = 0;
		}
		i++;
	}
	if (i)
	{
		tmp = ft_substr(str, 0, i);
		result = strjoin_and_free_both(result, tmp);
	}
	return (result);
}

int	print_envs_with_declaration(void)
{
	t_var		*tmp_var;
	char		*tmp;

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
				tmp = escape_string((char *)tmp_var->value);
				ft_putstr_fd(tmp, STDOUT_FILENO);
				free(tmp);
				write(STDOUT_FILENO, "\"", 1);
			}
			write(STDOUT_FILENO, "\n", 1);
		}
		tmp_var = tmp_var->next;
	}
	return (0);
}
