#include "libft/libft.h"
#include "env.h"
#include "utils.h"

/*
 * Get the variables with key substr(str, 0, env_len)
 *   and concatenate them to result.
 * Old result will be freed
 *   so caller doesn't need to free result after calling this function.
 */
static char	*expand_env_and_join(char *result,
	char *str, int env_len)
{
	char	*keyname;
	char	*env_val;
	char	*tmp_result;

	keyname = ft_substr(str, 0, env_len);
	if (!keyname)
		return (NULL);
	env_val = get_env_val(keyname);
	if (env_val)
	{
		if (result)
		{
			tmp_result = result;
			result = ft_strjoin(result, env_val);
			free(tmp_result);
		}
		else
			result = ft_strdup(env_val);
		free(env_val);
	}
	free(keyname);
	return (result);
}

/*
 * Concatenate string which doesn't have variable in it to result.
 * Old result will be freed
 *   so caller doesn't need to free result after calling this function.
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
 * Returns true if variable starts or ends.
 *
 * ex: It returns true in the following cases.
 *   - is_in_env == true and str[len] is invalid character as variable key.
 *   - is_in_env == false and str[len] is '$' and
 *       srt[len] is invalid character as variable key.
 *   - str[len] has reached to the end of string.
 */
static bool	will_toggle_env(bool is_in_env,
	bool is_in_noexpand, char *str, int len)
{
	bool	will_start_env;
	bool	will_end_env;

	will_start_env = !is_in_noexpand && str[len] == '$'
		&& (len < (int)ft_strlen(str)
			&& (ft_isalnum(str[len + 1])
				|| str[len + 1] == '_' || str[len + 1] == '?'));
	will_end_env = is_in_env
		&& (!(ft_isalnum(str[len]) || str[len] == '_'
				|| (len == 0 && str[len] == '?'))
			|| (len == 1 && str[len - 1] == '?'));
	return (will_start_env || will_end_env || !str[len]);
}

/*
 * string or expanded variable is joined to result
 *
 * How the function works depends on is_in_env.
 *   - is_in_env == true: expand variable and join to result.
 *   - is_in_env == false: join to result with out variable expansion.
 *
 * return: whether continue string processing.
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

/* Expand variables in string.
 *
 * ex:
 *   in($ABC=" abc def "):  |"$ABC"'\'$ABC'|
 *   out:                   |" abc def "'\'$ABC'|
 * ex:
 *   in($ABC="hoge"):       |'$''$'"ABC"'\'"$ABC""$ABC"|
 *   out:                   |'$''$'"ABC"'\'"hoge""hoge"|
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
		if (will_toggle_env(is_in_env, is_in_noexpand, str, len))
			is_continue = join_str_or_env(&result, &str, &len, &is_in_env);
		else
			len++;
	}
	return (result);
}

/* Expand variables in document of heredoc.
 *
 * ex:
 *   in($ABC="hoge"):       |\$USER \\$USER $USER '$USER' "$USER" \"\'\$|
 *   out:                   |$USER \jun jun 'jun' "jun" \"\'$|
 */
char	*expand_heredoc_document(char *str)
{
	char	*result;
	int		len;  // 現在見ている範囲の長さ. 先頭は*strを直接動かす
	bool	is_in_env;  // 現在環境変数にいるか
	char	*tmpstr;
	bool	is_continue;

	result = ft_strdup("");
	is_continue = true;
	len = 0;
	is_in_env = false;
	while (is_continue)
	{
		// エスケープの処理
		if (str[len] == '\\')
		{
			if (is_in_env)
				is_continue = join_str_or_env(&result, &str, &len, &is_in_env);
			// ここまでの文字列を繋げる
			// バックスラッシュの次の文字が特殊文字('$' | '\')じゃなければバックスラッシュも入れる
			tmpstr = ft_substr(str, 0, len + !(str[len + 1] == '$' || str[len + 1] == '\\'));
			result = strjoin_and_free_both(result, tmpstr);
			str += len + 1;
			// バックスラッシュの次の文字を繋げる
			tmpstr = ft_substr(str, 0, 1);
			result = strjoin_and_free_both(result, tmpstr);
			str += 1;
			len = 0;
		}
		// ドルマーク($) 環境変数開始の処理
		else if (will_toggle_env(is_in_env, false, str, len))
			is_continue = join_str_or_env(&result, &str, &len, &is_in_env);
		else
			len++;
	}
	return (result);
}
