#include "minishell.h"

/*
 * from から len文字substrして dst にくっつける
 */
static char	*substr_and_join(char *dst, char *from, int len)
{
	char *tmp_dst;
	char *tmp_substr;

	tmp_dst = dst;
	tmp_substr = ft_substr(from, 0, len);
	if (dst)
		dst = ft_strjoin(dst, tmp_substr);
	else
		dst = ft_strdup(tmp_substr);
	free(tmp_dst);
	free(tmp_substr);
	return (dst);
}

/*
 * 環境変数展開した文字列を分解して返す
 */
char	**split_expanded_str(char *str)
{
	char	**result;
	char	quote_char;
	int		len;
	char	*text;

	quote_char = '\0';
	result = NULL;
	while (*str)
	{
		// 空白飛ばし
		while (*str && *str == ' ')
			str++;
		if (!*str)
			break ;
		len = 0;
		text = NULL;
		while (true)
		{
			// クオートの中じゃない時の空白は区切り
			if ((!quote_char && str[len] == ' ') || !str[len])
			{
				text = substr_and_join(text, str, len);
				str += len;
				break;
			}
			if ((str[len] == '\'' || str[len] == '\"') && !(len > 0 && str[len - 1] == '\\'))
			{
				if (quote_char)
					quote_char = '\0';
				else
					quote_char = str[len];
				text = substr_and_join(text, str, len);
				str += len + 1;  // クオートの次の文字
				len = 0;
			}
			else
				len++;
		}
		char	**tmp = result;
		result = (char **)ptrarr_add_ptr((void **)tmp, text);
		free(tmp);
	}
	return (result);
}
