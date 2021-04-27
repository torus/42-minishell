#include "minishell.h"

/*
 * 環境変数を展開してresultに連結させる
 */
static	char *expand_and_join_env(char *result, char *str, int env_start_idx, int env_len)
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
static	char *result_join_normal_str(char *result, char *str, int start_idx, int len)
{
	char *tmp;
	// ここまでの文字列をresultに格納
	tmp = ft_substr(str, start_idx, len);  // '$' を含まない
	if (!tmp)
		return (NULL);
	if (!result)
		result = tmp;
	else
	{
		char *tmp2 = result;
		result = ft_strjoin(result, tmp);
		free(tmp);
		free(tmp2);
	}
	return (result);
}

/* 環境変数を展開する
 *
 * エスケープされたクオートなどはそのままなので, この後別の関数で処理してちょ
 * ex:
 *   in($ABC=" abc def "):  |"$ABC"'\'$ABC'|
 *   out:                   |" abc def "'\' abc def '|
 */
char	*expand_env_var(char *str)
{
	int		i;
	int		start_idx;
	char	*result;
	bool	is_in_env;
	bool	is_in_noexpand;  // 'の中にいるか?

	i = 0;
	start_idx = 0;
	result = NULL;
	is_in_env = false;
	is_in_noexpand = false;
	while (i <= (int)ft_strlen(str))
	{
		if (str[i] == '\'' && !(i > 0 && str[i - 1] == '\\'))  // シングルクオートで囲まれた中は環境変数展開しない
			is_in_noexpand = !is_in_noexpand;
		if (is_in_env && (!(ft_isalnum(str[i]) || str[i] == '_') || !str[i]))
		{
			result = expand_and_join_env(result, str, start_idx, i - start_idx);
			start_idx = i;  // ノーマル文字列が始まるidx
			is_in_env = false;
		}
		else if ((!is_in_noexpand && str[i] == '$' && (ft_isalnum(str[i + 1]) || str[i + 1] == '_')) || !str[i])  // 環境変数名が始まる!
		{
			result = result_join_normal_str(result, str, start_idx, i - start_idx);
			start_idx = i + 1;  // 環境変数名の始まるidx
			is_in_env = true;
			i++;
		}
		else
			i++;
	}
	return (result);
}
