#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include "libft/libft.h"
#include "builtin.h"
#include "env.h"
#include "minishell.h"
#include "utils.h"

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

static char	*escape_string(char *str)
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

/*
 * bashの挙動を見る限り,
 * "key=value"  (valueは空文字列でもOK) の場合のみ
 * 環境変数をセットしてるっぽい
 *
 * arg: "HOGE" とか "HOGE=" "HOGE=FUGA" とか
 */
static int	export_env(char *arg)
{
	char	**kvarr;
	char	*tmp;

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
	else if (kvarr[1])
	{
		tmp = kvarr[1];
		kvarr[1] = escape_string(kvarr[1]);
		free(tmp);
		ft_setenv(kvarr[0], kvarr[1], 0);
	}
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
