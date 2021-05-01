#include "builtin.h"

bool	is_builtin_command(char *cmd_name)
{
	if (ft_strncmp(cmd_name, "echo", 5) == 0
		|| ft_strncmp(cmd_name, "cd", 3) == 0
		|| ft_strncmp(cmd_name, "pwd", 4) == 0
		|| ft_strncmp(cmd_name, "env", 4) == 0)
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
	if (ft_strncmp(cmd_name, "cd", 3) == 0)
		return (builtin_cd);
	if (ft_strncmp(cmd_name, "pwd", 4) == 0)
		return (builtin_pwd);
	if (ft_strncmp(cmd_name, "env", 4) == 0)
		return (builtin_env);
	return (NULL);
}
