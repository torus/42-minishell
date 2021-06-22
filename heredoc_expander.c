#include "libft/libft.h"
#include "minishell.h"
#include "env.h"
#include "execution.h"
#include "utils.h"

static void	process_escape(char **result, char **str, int *len)
{
	char	*tmpstr;

	tmpstr = ft_substr(*str, 0,
			*len + !((*str)[*len + 1] == '$' || (*str)[*len + 1] == '\\'));
	check_malloc_has_succeeded("heredoc expander", tmpstr);
	*result = strjoin_and_free_both(*result, tmpstr);
	check_malloc_has_succeeded("heredoc expander", *result);
	*str += *len + 1;
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
	int		len;
	bool	is_in_env;
	bool	is_continue;

	result = ft_strdup("");
	check_malloc_has_succeeded("heredoc expander", result);
	is_continue = true;
	len = 0;
	is_in_env = false;
	while (is_continue)
	{
		if (str[len] == '\\')
		{
			if (is_in_env)
				is_continue = exp_join_str_or_env(&result, &str, &len,
						&is_in_env);
			process_escape(&result, &str, &len);
		}
		else if (exp_will_toggle_env(is_in_env, false, str, len))
			is_continue = exp_join_str_or_env(&result, &str, &len, &is_in_env);
		else
			len++;
	}
	return (result);
}
