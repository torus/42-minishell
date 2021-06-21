#include "minishell.h"
#include "utils.h"

/*
 * Convert AST string node to string for redirection.
 * TODO: 環境変数展開はここでする
 */
char	*expand_strnode4red(t_parse_node_string *string_node)
{
	char	*result;

	result = ft_strdup("");
	if (!result)
		put_minish_err_msg_and_exit(1, "string_node2string_for_red()",
			"malloc() failed");
	while (string_node)
	{
		if (string_node->type == TOKTYPE_NON_EXPANDABLE)
			result = strjoin_and_free_first(result, string_node->text);
		else
			result = expand_env_var(string_node->text);
		if (!result)
			put_minish_err_msg_and_exit(1, "string_node2string_for_red()",
				"malloc() failed");
		if (string_node->next)
			string_node = string_node->next->content.string;
		else
			break ;
	}
	return (result);
}
