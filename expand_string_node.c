#include "minishell.h"

typedef struct	s_cmd_str_node {
	char					*text;
	t_token_type			type;
	struct s_cmd_str_node	*next;
}				t_cmd_str_node;

static t_cmd_str_node	*cmd_str_node_add_back(t_cmd_str_node **str_node, char *text, t_token_type tok_type)
{
	t_cmd_str_node	*current_node;
	t_cmd_str_node	*new_node;

	new_node = ft_calloc(1, sizeof(t_cmd_str_node));
	if (!new_node)
		return (NULL);
	new_node->text = text;
	new_node->type = tok_type;
	if (!*str_node)
	{
		*str_node = new_node;
		return (new_node);
	}
	current_node = *str_node;
	while (current_node->next)
		current_node = current_node->next;
	current_node->next = new_node;
	return (new_node);
}

/* ast_str_node を cmd_str_node に変換する(環境変数は展開する).
 */
static t_cmd_str_node	*ast_str2cmd_str(t_parse_node_string *str_node)
{
	t_cmd_str_node	*result;
	char			*expanded_str;

	result = NULL;
	while (str_node)
	{
		if (str_node->type == TOKTYPE_NON_EXPANDABLE)
			expanded_str = ft_strdup(str_node->text);
		else
			expanded_str = expand_env_var(str_node->text);
		if (!cmd_str_node_add_back(&result, expanded_str, str_node->type))
				return (NULL);
		if (str_node->next)
			str_node = str_node->next->content.string;
		else
			str_node = NULL;
	}
	return (result);
}

/* 中間表現構造体を文字列配列にする. */
static char	**cmd_str2str_arr(t_cmd_str_node *str_node)
{
	char	**result;
	int		last_idx;

	last_idx = -1;  // resultの最後の文字列への添字
	result = NULL;
	while (str_node)
	{
		// 前の文字列の右端と今回の文字列左端に空白が無ければくっつける
		if (str_node->type == TOKTYPE_EXPANDABLE_QUOTED
			|| str_node->type == TOKTYPE_NON_EXPANDABLE
			&& last_idx >= 0
			&& result[last_idx][ft_strlen(result[last_idx]) - 1] != ' '
			&& str_node->text[0] != ' ')
		{
			char	*tmp = result[last_idx];
			result[last_idx] = ft_strjoin(tmp, str_node->text);
			free(tmp);
		}
		else
		{
			char	**tmp = result;
			result = (char **)ptrarr_add_ptr((void **)result, ft_strdup(str_node->text));
			free(tmp);
			last_idx = ptrarr_len((void **)result) - 1;
		}
		str_node = str_node->next;
	}
	return (result);
}

char	**expand_string_node(t_parse_node_string *string_node)
{
	// string_nodeを中間表現構造体(テキストとタイプを持つ)の配列に変換する.
	t_cmd_str_node *cmd_str = ast_str2cmd_str(string_node);
	printf("%p\n", cmd_str);
	// 中間表現構造体を文字列配列にする.
	char	**result = cmd_str2str_arr(cmd_str);

	while (cmd_str)
	{
		free(cmd_str->text);
		t_cmd_str_node *tmp = cmd_str;
		cmd_str = cmd_str->next;
		free(tmp);
	}
	return (result);
}
