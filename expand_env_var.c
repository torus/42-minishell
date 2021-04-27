#include "minishell.h"

/*
 * 環境変数を展開してresultに連結させる
 */
static char	*expand_env_and_join(char *result,
	char *str, int env_start_idx, int env_len)
{
	char	*keyname;
	char	*keyval;
	char	*tmp_result;

	keyname = ft_substr(str, env_start_idx, env_len);
	if (!keyname)
		return (NULL);
	keyval = get_env_val(keyname);
	if (keyval)
	{
		if (result)
		{
			tmp_result = result;
			result = ft_strjoin(result, keyval);
			free(keyval);
			free(tmp_result);
		}
		else
			result = keyval;
	}
	free(keyname);
	return (result);
}

/*
 * 普通の文字列をresultに連結させる
 */
static char	*result_join_normal_str(char *result,
	char *str, int start_idx, int len)
{
	char	*tmp;
	char	*tmp2;

	tmp = ft_substr(str, start_idx, len);
	if (!tmp)
		return (NULL);
	if (!result)
		result = tmp;
	else
	{
		tmp2 = result;
		result = ft_strjoin(result, tmp);
		free(tmp);
		free(tmp2);
	}
	return (result);
}

static bool	will_start_env_var(bool is_in_noexpand, char *str, int len)
{
	return ((!is_in_noexpand
				&& str[len] == '$'
				&& (len < (int)ft_strlen(str) && (ft_isalnum(str[len + 1])
				|| str[len + 1] == '_')))
				|| !str[len]);
}

/* 環境変数を展開する
 *
 * エスケープされたクオートなどはそのままなので, この後別の関数で処理してください
 * ex:
 *   in($ABC=" abc def "):  |"$ABC"'\'$ABC'|
 *   out:                   |" abc def "'\'$ABC'|
 */
char	*expand_env_var(char *str)
{
	int		len;
	char	*result;
	bool	is_in_env;
	bool	is_in_noexpand;

	len = 0;
	result = NULL;
	is_in_env = false;
	is_in_noexpand = false;
	while (true)
	{
		if (str[len] == '\'' && !(len > 0 && str[len - 1] == '\\'))
			is_in_noexpand = !is_in_noexpand;
		if (is_in_env && (!(ft_isalnum(str[len]) || str[len] == '_') || !str[len]))
		{
			result = expand_env_and_join(result, str, 0, len);
			if (!str[len])
				break;
			str += len;
			len = 0;
			is_in_env = false;
		}
		else if (will_start_env_var(is_in_noexpand, str, len))
		{
			result = result_join_normal_str(result, str, 0, len);
			if (!str[len])
				break;
			str += len + 1;
			is_in_env = true;
			len = 0;
		}
		else
			len++;
	}
	return (result);
}
