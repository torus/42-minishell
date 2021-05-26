#include "minishell.h"

/*
 * from から len文字substrして dst にくっつける
 */
static const char	*substr_and_join(const char *dst, const char *from, int len)
{
	const char	*tmp_dst;
	const char	*tmp_substr;

	tmp_dst = dst;
	tmp_substr = ft_substr(from, 0, len);
	if (dst)
		dst = ft_strjoin(dst, tmp_substr);
	else
		dst = ft_strdup(tmp_substr);
	free((void *)tmp_dst);
	free((void *)tmp_substr);
	return (dst);
}

/*
 * get_str_from_expanded_str() 内でのクオートが見つかった場合の
 *   text, str, len の更新処理.
 * Normの行数対策で切り分けた.
 *
 * quote_char: 文字列ではなく,
 *   呼び出し元でのchar型変数のポインタ.
 *   クオーテーションの文字( ' or " )もしくは\0が入ってる
 * text: 空白で区切られた文字.
 *   str から取り出した部分文字列をくっつけて返す.
 * str: 変数展開された文字列.
 *   この関数内で len+1 ポインタが進む
 * len: strからlen文字strdupする 時に使う
 *   この関数内で 0 が代入される.
 *
 * return: strからlen文字strdupしたものをくっつけたtext
 */
static const char	*update_text_and_str(char *quote_char,
	const char *text, char **str, int *len)
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
 * 環境変数展開された文字列から空白区切りされた文字列の1単位をstrdupする
 *
 * in: |abc def"ghi jkl"| だとしたら
 * out: |abc| を返し, *str を d まで進める
 * 次の呼び出しでは |defghi jkl| を返し, *str を \0 まで進める
 */
static const char	*get_str_from_expanded_str(char **str)
{
	char	quote_char;
	int		len;
	const char	*text;

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
 * 環境変数展開した文字列を分解して返す
 *
 * ex:
 *   in($ABC="hoge"):       |'$''$'"ABC"'\'"$ABC""$ABC"|
 *   out:                   ["$$ABC\hogehoge"]
 */
const char	**split_expanded_str(char *str)
{
	const char	**result;
	const char	*text;
	const char	**tmp;

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
			result = (const char **)ptrarr_add_ptr((void **)tmp, (void *)text);
			free(tmp);
		}
	}
	return (result);
}
