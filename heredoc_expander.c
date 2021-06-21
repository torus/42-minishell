#include "libft/libft.h"
#include "minishell.h"
#include "env.h"
#include "execution.h"
#include "utils.h"

static void process_escape(char **result, char **str, int *len)
{
	char	*tmpstr;

	// ここまでの文字列を繋げる
	// バックスラッシュの次の文字が特殊文字('$' | '\')じゃなければバックスラッシュも入れる
	tmpstr = ft_substr(*str, 0, *len + !((*str)[*len + 1] == '$' || (*str)[*len + 1] == '\\'));
	check_malloc_has_succeeded("heredoc expander", tmpstr);
	*result = strjoin_and_free_both(*result, tmpstr);
	check_malloc_has_succeeded("heredoc expander", *result);
	*str += *len + 1;
	// バックスラッシュの次の文字を繋げる
	tmpstr = ft_substr(*str, 0, 1);
	check_malloc_has_succeeded("heredoc expander", tmpstr);
	*result = strjoin_and_free_both(*result, tmpstr);
	check_malloc_has_succeeded("heredoc expander", *result);
	*str += 1;
	*len = 0;
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
	bool	is_continue;

	result = ft_strdup("");
	check_malloc_has_succeeded("heredoc expander", result);
	is_continue = true;
	len = 0;
	is_in_env = false;
	while (is_continue)
	{
		// エスケープの処理
		if (str[len] == '\\')
		{
			if (is_in_env)
				is_continue = exp_join_str_or_env(&result, &str, &len, &is_in_env);
			process_escape(&result, &str, &len);
		}
		// ドルマーク($) 環境変数開始の処理
		else if (exp_will_toggle_env(is_in_env, false, str, len))
			is_continue = exp_join_str_or_env(&result, &str, &len, &is_in_env);
		else
			len++;
	}
	return (result);
}
