#include "minishell.h"
#include "utils.h"

static bool	cmd_str_node_add_back(t_cmd_str_node ***str_node_arr,
	const char *text, t_token_type tok_type)
{
	t_cmd_str_node	*new_node;
	t_cmd_str_node	**tmp;

	new_node = malloc(sizeof(t_cmd_str_node));
	if (!new_node)
		return (false);
	new_node->text = (char *)text;
	new_node->type = tok_type;
	tmp = *str_node_arr;
	*str_node_arr = (t_cmd_str_node **)ptrarr_add_ptr(
			(void **)*str_node_arr, new_node);
	free(tmp);
	if (!*str_node_arr)
		return (false);
	return (true);
}

/* ast_str_node を cmd_str_node に変換する(環境変数は展開する).
 */
static t_cmd_str_node	**ast_str2cmd_str(t_parse_node_string *str_node)
{
	t_cmd_str_node	**result;
	const char		*expanded_str;

	result = NULL;
	while (str_node)
	{
		if (str_node->type == TOKTYPE_NON_EXPANDABLE)
			expanded_str = ft_strdup(str_node->text);
		else
			expanded_str = expand_env_var(str_node->text);
		if (!expanded_str
			|| !cmd_str_node_add_back(&result, expanded_str, str_node->type))
			return (NULL);
		if (str_node->next)
			str_node = str_node->next->content.string;
		else
			str_node = NULL;
	}
	return ((t_cmd_str_node **)result);
}

/* TOKTYPE_EXPANDABLEを処理する
 *
 * 渡された next_str を更新する際はは内部でfreeする.
 *
 * result: 文字列配列変数へのポインタ
 *   (Normのためにvoidポインタにしている)
 * next_str: 次resultに追加される文字列を指すポインタ
 * text: str_node->text
 *
 * Return: 更新された next_str
 */
static char	*expandable_node2strarr(void ***result,
	char *next_str, char *text)
{
	int		len;

	len = 0;
	while (text[len])
	{
		if (text[len] == ' ' && len == 0)
			text++;
		else if (text[len] == ' ' && len)
		{
			next_str = strjoin_nullable_and_free_both(
					next_str, ft_substr(text, 0, len));
			*result = ptrarr_add_ptr_and_free(*result, next_str);
			next_str = NULL;
			if (!*result)
				return (NULL);
			text += len + 1;
			len = 0;
		}
		else
			len++;
	}
	if (len)
		next_str = strjoin_nullable_and_free_both(
				next_str, ft_substr(text, 0, len));
	return (next_str);
}

/* 中間表現構造体を文字列配列にする. */
static char	**cmd_str2str_arr(t_cmd_str_node **str_node)
{
	void	**result;
	char	*next_str;
	int		i;

	i = 0;
	result = NULL;
	next_str = NULL;
	while (str_node[i])
	{
		if (str_node[i]->type != TOKTYPE_EXPANDABLE)
		{
			if (next_str)
				next_str = strjoin_and_free_first(next_str, str_node[i]->text);
			else
				next_str = ft_strdup(str_node[i]->text);
		}
		else
			next_str = expandable_node2strarr(&result, next_str,
					str_node[i]->text);
		i++;
	}
	if (next_str)
		result = ptrarr_add_ptr_and_free(result, next_str);
	return ((char **)result);
}

/* t_parse_node_string を文字列配列に変換して返す */
char	**expand_string_node(t_parse_node_string *string_node)
{
	t_cmd_str_node	**cmd_str;
	int				i;
	char			**result;

	cmd_str = ast_str2cmd_str(string_node);
	if (!cmd_str)
		return (NULL);
	result = cmd_str2str_arr(cmd_str);
	i = 0;
	while (cmd_str[i])
	{
		free(cmd_str[i]->text);
		free(cmd_str[i]);
		i++;
	}
	free(cmd_str);
	return (result);
}
