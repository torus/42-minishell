#include "builtin.h"

bool	is_builtin_command(char *cmd_name)
{
	if (ft_strcmp(cmd_name, "echo") == 0
		|| ft_strcmp(cmd_name, "cd") == 0
		|| ft_strcmp(cmd_name, "pwd") == 0
		|| ft_strcmp(cmd_name, "env") == 0
		|| ft_strcmp(cmd_name, "export") == 0
		|| ft_strcmp(cmd_name, "unset") == 0
		|| ft_strcmp(cmd_name, "exit") == 0)
		return (true);
	return (false);
}

/*
 * ビルトインコマンド(関数)のポインタを返す.
 * これが上手くいけば is_builtin_command() はいらないかも
 */
t_builtin_cmd	*get_builtin_func(char *cmd_name)
{
	if (ft_strcmp(cmd_name, "echo") == 0)
		return (builtin_echo);
	if (ft_strcmp(cmd_name, "cd") == 0)
		return (builtin_cd);
	if (ft_strcmp(cmd_name, "pwd") == 0)
		return (builtin_pwd);
	if (ft_strcmp(cmd_name, "env") == 0)
		return (builtin_env);
	if (ft_strcmp(cmd_name, "export") == 0)
		return (builtin_export);
	if (ft_strcmp(cmd_name, "unset") == 0)
		return (builtin_unset);
	if (ft_strcmp(cmd_name, "exit") == 0)
		return (builtin_exit);
	return (NULL);
}
