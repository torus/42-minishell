#include "minishell.h"

static char	*substr_and_join(char *dst, char *from, int len)
{
	char	*tmp_dst;
	char	*tmp_substr;

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
 * This function is called when quote is found
 *   while processing get_str_from_expanded_str().
 * This function updates passed text, str and len.
 *
 * quote_char: Variable in get_str_from_expanded_str()
 *               this variable has quotation character '\'', '\"' or '\0'.
 * text: String that is delimited by space.
 *         It will be concatenate with sub string of str.
 * str: Variable expanded string.
 *        Advance the str pointer by len+1.
 * len: It's used for strdup().
 *        0 will be assigned in len in this function.
 *
 * return: text concatenated with len characters substring from str
 */
static char	*update_text_and_str(char *quote_char,
	char *text, char **str, int *len)
{
	if (*quote_char)
		*quote_char = '\0';
	else
		*quote_char = (*str)[*len];
	text = substr_and_join(text, *str, *len);
	*str += *len + 1;
	*len = 0;
	return (text);
}

/*
 * Duplicate one unit of space-delimited string
 *   from environment variable expanded string.
 *
 * str: pointer of string.
 * Return: one unit of space delimited string.
 *
 * ex:
 *   If str is |abc def"ghi jkl"|, return |abc| and advance *str to 'd'.
 *     In next call, return |defghi jkl| and advance *str to '\0'.
 */
static char	*get_str_from_expanded_str(char **str)
{
	char	quote_char;
	int		len;
	char	*text;

	quote_char = '\0';
	len = 0;
	text = NULL;
	while (true)
	{
		if ((!quote_char && (*str)[len] == ' ') || !(*str)[len])
		{
			text = substr_and_join(text, *str, len);
			*str += len;
			break ;
		}
		if (((*str)[len] == '\'' || (*str)[len] == '\"')
				&& !(len > 0 && (*str)[len - 1] == '\\'))
			text = update_text_and_str(&quote_char, text, str, &len);
		else
			len++;
	}
	return (text);
}

/*
 * Expand environment variables in string and split to array of string.
 *
 * ex:
 *   in($ABC="hoge"):       |'$''$'"ABC"'\'"$ABC""$ABC"|
 *   out:                   ["$$ABC\hogehoge"]
 */
char	**split_expanded_str(char *str)
{
	char	**result;
	char	*text;
	char	**tmp;

	result = NULL;
	while (*str)
	{
		while (*str && *str == ' ')
			str++;
		if (!*str)
			break ;
		text = get_str_from_expanded_str(&str);
		if (text)
		{
			tmp = result;
			result = (char **)ptrarr_add_ptr((void **)tmp, text);
			free(tmp);
		}
	}
	return (result);
}
