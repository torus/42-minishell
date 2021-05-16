#include "minishell.h"

/* 前の文字列の右端と今回の文字列左端に空白が無ければくっつける */
static bool argv_add_arg(char ***argv, char *arg)
{
	char	argv_len;

	if (!*argv)
	{
		*argv = (char **)ptrarr_add_ptr((void **)argv, arg);
		return (!!*argv);
	}
	argv_len = ptrarr_len((void **)(*argv));
	if ((*argv)[argv_len - 1][ft_strlen((*argv)[argv_len - 1]) - 1] != ' '
		&& arg[ft_strlen(arg) - 1] != ' ')
	{
		char	*tmp = (*argv)[argv_len - 1];
		(*argv)[argv_len - 1] = ft_strjoin((*argv)[argv_len - 1], arg);
		free(tmp);
	}
	else
	{
		char **tmp = (*argv);
		(*argv) = (char **)ptrarr_add_ptr((void **)(*argv), arg);
		free(tmp);
	}
	return (!!(*argv));
}

char	**expand_string_node(t_parse_node_string *string_node)
{
	char			**result;
	char			*expanded_str;

	result = NULL;
	while (string_node)
	{
		if (string_node->type == TOKTYPE_NON_EXPANDABLE)
			expanded_str = ft_strdup(string_node->text);
		else
			expanded_str = expand_env_var(string_node->text);
		// EXPANDABLE_QUOTED なら ft_split() して追加する
		if (string_node->type == TOKTYPE_EXPANDABLE)
		{
			char	**tmp = ft_split(expanded_str, ' ');
			free(expanded_str);
			int i = 0;
			while (tmp[i])
				argv_add_arg(&result, tmp[i++]);
			free(tmp);
		}
		else
			argv_add_arg(&result, expanded_str);
		if (string_node->next)
			string_node = string_node->next->content.string;
		else
			string_node = NULL;
	}
	return (result);
}

