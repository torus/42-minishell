#include <stdlib.h>
#include "libft/libft.h"
#include "env.h"
#include "minishell.h"

/*
 * keyname, value を元に "key=value" の文字列を作成する
 */
const char	*generate_kvstr(const char *key, const char *value)
{
	const char	*tmp;
	const char	*kvstr;

	tmp = ft_strjoin(key, "=");
	kvstr = ft_strjoin(tmp, value);
	free((void *)tmp);
	return (kvstr);
}

/*
 * 標準ライブラリの setenv() と同じ動作をする
 *
 * key: 環境変数のキー名
 * value: 環境変数の値
 * rewrite: 環境変数が既に存在している場合に書き換えるかどうか
 *
 * return: 正常なら0. それ以外なら-1.
 */
int	ft_setenv(const char *key, const char *value, int rewrite)
{
	t_var	*var;

	var = get_env(key);
	if (var)
	{
		free((void *)var->value);
		var->value = value;
		return (0);
	}
	var = malloc(sizeof(t_var));
	if (!var)
		return (-1);
	var->key = ft_strdup(key);
	var->value = ft_strdup(value);
	var->is_shell_var = 0;
	return (0);
}

/*
 * name で指定された環境変数が存在する場合, それを削除する.
 * 標準ライブラリの unsetenv() と同じ動作をする.
 *
 * name: 環境変数のキー名
 *
 * return: 正常なら0. それ以外なら-1.
 */
int	ft_unsetenv(const char *name)
{
	extern char	**environ;
	int			idx;
	int			env_len;
	int			name_len;

	idx = 0;
	name_len = ft_strlen(name);
	env_len = ptrarr_len((void **)environ);
	while (environ[idx])
	{
		if (ft_strncmp(environ[idx], name, name_len) == 0
			&& environ[idx][name_len] == '=')
		{
			if ((void *)environ[idx] < (void *)&idx)
				free(environ[idx]);
			ft_memmove(environ + idx, environ + idx + 1,
				sizeof(char *) * (env_len - idx));
			return (0);
		}
		idx++;
	}
	return (0);
}
