#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include "libft/libft.h"
#include "builtin.h"
#include "env.h"
#include "minishell.h"

static void	put_export_err_msg(char *identifer)
{
	char	*tmp;
	char	*errmsg;

	tmp = ft_strjoin("`", identifer);
	if (!tmp)
		put_minish_err_msg_and_exit(1, "export", "failed malloc()");
	errmsg = ft_strjoin(tmp, "': not a valid identifier");
	free(tmp);
	if (!errmsg)
		put_minish_err_msg_and_exit(1, "export", "failed malloc()");
	put_minish_err_msg("export", errmsg);
	free(errmsg);
}

static bool	is_valid_env_key(char *key)
{
	int	i;

	if (!ft_strlen(key))
		return (false);
	i = 0;
	while (key[i])
	{
		if ((!ft_isalnum(key[i]) && key[i] != '_' && key[i] != '+')
			|| (ft_isdigit(key[i]) && i == 0)
			|| (key[i] == '+' && (i == 0 || key[i + 1] != '\0')))
			return (false);
		i++;
	}
	return (true);
}

static int	export_strjoin_env(const char *key, const char *value)
{
	t_var	*env_var;
	char	*new_value;

	key = ft_substr(key, 0, ft_strchr(key, '+') - key);
	env_var = get_env(key);
	if (!env_var || !env_var->value || !ft_strlen(env_var->value))
		new_value = ft_strdup(value);
	else
		new_value = ft_strjoin(env_var->value, value);
	ft_setenv(key, new_value, 0);
	free((void *)key);
	free(new_value);
	return (0);
}

static int	export_env(char *arg)
{
	char	**kvarr;

	kvarr = split_first_c(arg, '=');
	if (!is_valid_env_key(kvarr[0])
		|| (ft_strchr(kvarr[0], '+') && !kvarr[1]))
	{
		put_export_err_msg(arg);
		free_ptrarr((void **)kvarr);
		return (1);
	}
	if (ft_strchr(kvarr[0], '+'))
		export_strjoin_env(kvarr[0], kvarr[1]);
	else
		ft_setenv(kvarr[0], kvarr[1], 0);
	free_ptrarr((void **)kvarr);
	return (0);
}

/*
 * builtin export command
 *
 * argv: ["export", "ABC=abc"]
 */
int	builtin_export(char **argv)
{
	int	status;

	if (ptrarr_len((void **)argv) < 2)
		return (print_envs_with_declaration());
	argv++;
	status = 0;
	while (*argv)
	{
		status |= export_env(*argv);
		argv++;
	}
	return (status);
}
