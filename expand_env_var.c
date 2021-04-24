#include "minishell.h"

/* 環境変数を展開する
 */
char	*expand_env_var(char *str)
{
	int		i;
	int		start_idx;
	char	*result;
	bool	is_in_env;

	i = 0;
	start_idx = 0;
	result = NULL;
	is_in_env = false;
	while (i <= (int)ft_strlen(str))
	{
		if (is_in_env && (!(ft_isalnum(str[i]) || str[i] == '_') || !str[i]))
		{
			char *keyname = ft_substr(str, start_idx, i - start_idx);
			if (!keyname)
				return (NULL);
			char *tmp = get_env_val(keyname);
			if (tmp)
			{
				char *tmp_result = result;
				result = ft_strjoin(result, tmp);
				free(tmp);
				free(tmp_result);
			}
			free(keyname);
			start_idx = i;  // ノーマル文字列が始まるidx
			is_in_env = false;
		}
		else if ((str[i] == '$' && (ft_isalnum(str[i + 1]) || str[i + 1] == '_')) || !str[i])  // 環境変数名が始まる!
		{
			char *tmp;
			// ここまでの文字列をresultに格納
			tmp = ft_substr(str, start_idx, i - start_idx);  // '$' を含まない
			if (!tmp)
				return (NULL);
			if (!result)
			{
				result = tmp;
			}else{
				char *tmp2 = result;
				result = ft_strjoin(result, tmp);
				free(tmp);
				free(tmp2);
			}
			start_idx = i + 1;  // 環境変数名の始まるidx
			is_in_env = true;
		}
		i++;
	}
	return (result);
}
