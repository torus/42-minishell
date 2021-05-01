#include "builtin.h"

bool	is_builtin_command(char *cmd_name)
{
	if (ft_strncmp(cmd_name, "echo", 5) == 0)
		return (true);
	return (false);
}

/*
 * ビルトインコマンド(関数)のポインタを返す.
 * これが上手くいけば is_builtin_command() はいらないかも
 */
t_builtin_cmd	*get_builtin_func(char *cmd_name)
{
	if (ft_strncmp(cmd_name, "echo", 5) == 0)
		return (builtin_echo);
	return (NULL);
}
