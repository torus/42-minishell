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

	// `=': not a valid identifier
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

static int	print_envs_with_declaration(void)
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

static bool	is_valid_env_key(char *key)
{
	int	i;

	if (!ft_strlen(key))
		return (false);
	i = 0;
	while (key[i])
	{
		if ((!ft_isalnum(key[i]) && key[i] != '_')
			|| (ft_isdigit(key[i]) && i == 0)
			|| (key[i] == '+' && (i == 0 || key[i+1] != '\0')))
			return (false);
		i++;
	}
	return (true);
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

	if (!ft_strchr(arg, '='))
		return (0);
	kvarr = split_first_c(arg, '=');
	if (!is_valid_env_key(kvarr[0]))
	{
		put_export_err_msg(arg);
		free_ptrarr((void **)kvarr);
		return (0);
	}
	if (ft_strchr(kvarr[0], '+'))
		export_update_env(kvarr[0], kvarr[1]);
	else
		ft_setenv(kvarr[0], kvarr[1], 1);
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
	if (ptrarr_len((void **)argv) < 2)
		return (print_envs_with_declaration());
	argv++;
	while (*argv)
	{
		export_env(*argv);
		argv++;
	}
	return (0);
}
