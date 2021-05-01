#include "minishell.h"

bool	is_buildin_command(char *cmd_name)
{
	if (ft_strncmp(cmd_name, "echo", 5) == 0)
		return (true);
	return (false);
}

