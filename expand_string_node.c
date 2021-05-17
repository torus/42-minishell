#include "minishell.h"
#include "utils.h"

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

static void	cmd_str_expandable2str_arr(char ***result, char **next_str, t_cmd_str_node *str_node)
{
	int		len;
	char	*str;
	char	**tmparr;

	len = 0;
	str = str_node->text;
	while (str[len])
	{
		if (str[len] == ' ' && len == 0)
			str++;
		else if (str[len] == ' ' && len)
		{
			*next_str = strjoin_and_free_both(*next_str, ft_substr(str, 0, len));
			tmparr = *result;
			*result = (char **)ptrarr_add_ptr((void **)*result, *next_str);
			free(tmparr);
			*next_str = ft_strdup("");
			str += len + 1;
			len = 0;
		}
		else
			len++;
	}
	if (len)
		*next_str = strjoin_and_free_both(*next_str, ft_substr(str, 0, len));
}

/* 中間表現構造体を文字列配列にする. */
static char	**cmd_str2str_arr(t_cmd_str_node **str_node)
{
	char	**result;
	char	*next_str;  // 次resultに追加される文字列
	int		i;
	char	**tmparr;

	i = 0;
	result = NULL;
	next_str = ft_strdup("");
	while (str_node[i])
	{
		if (str_node[i]->type != TOKTYPE_EXPANDABLE)
			next_str = strjoin_and_free_first(next_str, str_node[i]->text);
		else
			cmd_str_expandable2str_arr(&result, &next_str, str_node[i]);
		i++;
	}
	if (next_str && ft_strlen(next_str))
	{
		tmparr = result;
		result = (char **)ptrarr_add_ptr((void **)result, next_str);
		free(tmparr);
	}
	else
		free(next_str);
	return (result);
}

char	**expand_string_node(t_parse_node_string *string_node)
{
	// string_nodeを中間表現構造体(テキストとタイプを持つ)の配列に変換する.
	t_cmd_str_node **cmd_str = ast_str2cmd_str(string_node);

	int i = 0;
	printf("cmd_str:\n");
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
	printf("result:\n");
	while (result && result[i])
	{
		printf("result[%d]: |%s|\n", i, result[i]);
		i++;
	}

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
