#include "minishell.h"
#include "utils.h"

static void	string_node2string_malloc_check(char *result)
{
	if (!result)
		put_minish_err_msg_and_exit(1, "strnode2str", "malloc() failed");
}

/*
 * Convert AST string node to string.
 */
char	*string_node2string(t_parse_node_string *string_node,
	bool add_quotes)
{
	char	*result;

	result = ft_strdup("");
	string_node2string_malloc_check(result);
	while (string_node)
	{
		if (add_quotes && string_node->type == TOKTYPE_NON_EXPANDABLE)
			result = strjoin_and_free_first(result, "\'");
		else if (add_quotes && string_node->type == TOKTYPE_EXPANDABLE_QUOTED)
			result = strjoin_and_free_first(result, "\"");
		string_node2string_malloc_check(result);
		result = strjoin_and_free_first(result, string_node->text);
		string_node2string_malloc_check(result);
		if (add_quotes && string_node->type == TOKTYPE_NON_EXPANDABLE)
			result = strjoin_and_free_first(result, "\'");
		else if (add_quotes && string_node->type == TOKTYPE_EXPANDABLE_QUOTED)
			result = strjoin_and_free_first(result, "\"");
		string_node2string_malloc_check(result);
		if (string_node->next)
			string_node = string_node->next->content.string;
		else
			break ;
	}
	return (result);
}
