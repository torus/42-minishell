#include "minishell.h"

/*
 * 一度文字列を全て展開した上でexpanded_strarrを生成する
 */
char	**expand_string_node(t_parse_node_string *string_node)
{
	char	*restored_str;
	char	*expanded_str;
	char	**splitted_expanded_str;

	restored_str = string_node2string(string_node);
	expanded_str = expand_env_var(restored_str);
	free(restored_str);
	splitted_expanded_str = split_expanded_str(expanded_str);
	free(expanded_str);
	return (splitted_expanded_str);
}
