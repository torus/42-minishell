#include <stdlib.h>
#include "libft/libft.h"
#include "env.h"
#include "minishell.h"

/*
 * Generate "key=value" format string from key and value.
 */
char	*generate_kvstr(const char *key, const char *value)
{
	char	*tmp;
	char	*kvstr;

	tmp = ft_strjoin(key, "=");
	kvstr = ft_strjoin(tmp, value);
	free(tmp);
	return (kvstr);
}

/* Set or update variable.
 *
 * key: Key name of variable. This argument isn't nullable.
 * value: Value of variable. This argument is nullable.
 * is_shell_var: Whether the variable is shell variable or environment variable.
 *
 * Return: 0 if the process is succeeded, otherwise returns -1.
 */
int	ft_setenv(const char *key, const char *value, bool is_shell_var)
{
	t_var	*var;

	var = get_env(key);
	if (var)
	{
		if (value)
		{
			free((void *)var->value);
			var->value = ft_strdup(value);
		}
		var->is_shell_var = is_shell_var;
	}
	else if (!add_new_var(&g_shell.vars, key, value, is_shell_var))
		return (-1);
	return (0);
}

/*
 * Remove variable if the variable specified by key exist.
 *
 * key: Key name of variable.
 *
 * Return: 0 if the process is succeeded, otherwise returns 1.
 *
 * NOTE: Returns 0 even if variable specified by key doesn't exist.
 */
int	ft_unsetenv(const char *key)
{
	t_var	*prev;
	t_var	*current;

	current = g_shell.vars;
	prev = NULL;
	while (current)
	{
		if (!ft_strcmp(current->key, key))
		{
			if (prev)
				prev->next = current->next;
			else
				g_shell.vars = current->next;
			free((void *)current->key);
			free((void *)current->value);
			free(current);
			break ;
		}
		prev = current;
		current = current->next;
	}
	return (0);
}
