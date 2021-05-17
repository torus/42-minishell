#include "minishell.h"

typedef struct	s_cmd_str_node {
	char					*text;
	t_token_type			type;
}				t_cmd_str_node;

static bool	cmd_str_node_add_back(t_cmd_str_node ***str_node_arr, char *text, t_token_type tok_type)
{
	t_cmd_str_node	*new_node;
	t_cmd_str_node	**tmp;

	new_node = malloc(sizeof(t_cmd_str_node));
	if (!new_node)
		return (false);
	new_node->text = text;
	new_node->type = tok_type;
	tmp = *str_node_arr;
	*str_node_arr = (t_cmd_str_node **)ptrarr_add_ptr((void **)*str_node_arr, new_node);
	free(tmp);
	return (true);
}

/* ast_str_node を cmd_str_node に変換する(環境変数は展開する).
 */
static t_cmd_str_node	**ast_str2cmd_str(t_parse_node_string *str_node)
{
	t_cmd_str_node	**result;
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
	return ((t_cmd_str_node **)result);
}

/* 中間表現構造体を文字列配列にする. */
static char	**cmd_str2str_arr(t_cmd_str_node **str_node)
{
	char	**result;
	int		i;

	i = 0;
	result = NULL;
	while (str_node[i])
	{
		// 前の文字列の右端と今回の文字列左端に空白が無ければくっつける
		int j = 0;
		while (str_node[i]->text[j])
		{
			
		}
		i++;
	}
	return (result);
}

char	**expand_string_node(t_parse_node_string *string_node)
{
	// string_nodeを中間表現構造体(テキストとタイプを持つ)の配列に変換する.
	t_cmd_str_node **cmd_str = ast_str2cmd_str(string_node);

	int i = 0;
	while (cmd_str[i])
	{
		printf("%d:\n", i);
		printf("\t|%s|\n", cmd_str[i]->text);
		if (cmd_str[i]->type == TOKTYPE_EXPANDABLE)
			printf("\tTOKTYPE_EXPANDABLE\n");
		else if (cmd_str[i]->type == TOKTYPE_EXPANDABLE_QUOTED)
			printf("\tTOKTYPE_EXPANDABLE_QUOTED\n");
		else if (cmd_str[i]->type == TOKTYPE_NON_EXPANDABLE)
			printf("\tTOKTYPE_NON_EXPANDABLE\n");
		else
			printf("\tUNKNOWN: %d\n", cmd_str[i]->type);
		i++;
	}
	// 中間表現構造体を文字列配列にする.
	char	**result = cmd_str2str_arr(cmd_str);

	i = 0;
	while (cmd_str[i])
	{
		free(cmd_str[i]->text);
		free(cmd_str[i]);
		i++;
	}
	return (result);
}
