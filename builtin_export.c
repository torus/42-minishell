#include "builtin.h"

static int	print_envs_with_declaration()
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
		if (!env_val)
		{
			free_ptrarr((void **)kvarr);
			return (ERROR);
		}

		write(STDOUT_FILENO, "declare -x ", ft_strlen( "declare -x "));
		// print key=
		write(STDOUT_FILENO, environ[i], ft_strchr(environ[i], '=') - environ[i] + 1);
		write(STDOUT_FILENO, "\"", 1);
		ft_putstr_fd(env_val, STDOUT_FILENO);
		write(STDOUT_FILENO, "\"\n", 2);
		i++;
	}
	return (0);
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
