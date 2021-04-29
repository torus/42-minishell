#include "minishell.h"

/*
 * 環境変数を展開してresultに連結させて引数をfreeする
 */
static char	*expand_env_and_join(char *result,
	char *str, int env_len)
{
	char	*keyname;
	char	*keyval;
	char	*tmp_result;

	keyname = ft_substr(str, 0, env_len);
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
 * 普通の文字列をresultに連結させて引数をfreeする
 */
static char	*result_join_normal_str(char *result,
	char *str, int len)
{
	char	*tmp;
	char	*tmp2;

	tmp = ft_substr(str, 0, len);
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

/*
 * str[len] から環境変数が開始するかどうか
 */
static bool	will_start_env_var(bool is_in_noexpand, char *str, int len)
{
	return ((!is_in_noexpand && str[len] == '$'
			&& (len < (int)ft_strlen(str)
				&& (ft_isalnum(str[len + 1]) || str[len + 1] == '_' || str[len + 1] == '?')))
		|| !str[len]);
}

/*
 * 文字列(or 環境変数)を連結する
 *
 * is_in_env の値によって処理が変わる.
 *   is_in_env == true: 環境変数展開して連結する.
 *   is_in_env == false: 普通に連結する.
 *
 * return: 文字列解析処理を続けるかどうか (is_continue)
 */
static bool	join_str_or_env(char **result,
	char **str, int *len, bool *is_in_env)
{
	if (*is_in_env)
		*result = expand_env_and_join(*result, *str, *len);
	else
		*result = result_join_normal_str(*result, *str, *len);
	if (!(*str)[*len] || !result)
		return (false);
	*str += *len + !*is_in_env;
	*len = 0;
	*is_in_env = !*is_in_env;
	return (true);
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
	bool	is_continue;

	len = 0;
	is_continue = true;
	result = NULL;
	is_in_env = false;
	is_in_noexpand = false;
	while (is_continue)
	{
		if (str[len] == '\'' && !(len > 0 && str[len - 1] == '\\'))
			is_in_noexpand = !is_in_noexpand;
		if ((is_in_env
				&& (!(ft_isalnum(str[len]) || str[len] == '_' || (len == 0 && str[len] == '?')) || (len == 1 && str[len-1] == '?') || !str[len]))
			|| will_start_env_var(is_in_noexpand, str, len))
			is_continue = join_str_or_env(&result, &str, &len, &is_in_env);
		else
			len++;
	}
	return (result);
}
