#include <stdlib.h>
#include "libft/libft.h"
#include "env.h"
#include "minishell.h"

/*
 * keyname, value を元に "key=value" の文字列を作成する
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

/* 環境変数(or シェル変数)に値をセットする
 * key: 環境変数のキー名
 * value: 環境変数の値
 * is_shell_var: 環境変数が既に存在している場合に書き換えるかどうか
 *
 * return: 正常なら0. それ以外なら-1.
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
	else
		add_new_var(&g_shell.vars, key, value, is_shell_var);
	return (0);
}

/*
 * key で指定された環境変数が存在する場合, それを削除する.
 * 標準ライブラリの unsetenv() と同じ動作をする.
 *
 * key: 環境変数のキー名
 *
 * return: 正常なら0. それ以外なら-1.
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
			break;
		}
		prev = current;
		current = current->next;
	}
	return (0);
}
