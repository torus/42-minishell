#include "builtin.h"

int	builtin_pwd(char **argv)
{
	char	buf[PATH_MAX];

	if (getcwd(buf, PATH_MAX) == NULL)
		return (put_err_msg_and_ret("getcwd() failed"));
	ft_putendl_fd(buf, STDOUT_FILENO);
	return (0);
}
