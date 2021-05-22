#include <limits.h>
#include "minishell.h"
#include "builtin.h"
#include "env.h"
#include "libft/libft.h"

static void	put_exit_errmsg_and_exit(char *exit_status)
{
	char	*tmp;

	tmp = ft_strjoin(exit_status, ": numeric argument required");
	if (!tmp)
		put_minish_err_msg_and_exit(2, "exit", "ft_strjoin() failed");
	put_minish_err_msg_and_exit(2, "exit", tmp);
}

static void	exit_atol(char *str)
{
	long	num;

	if (!ft_atol(str, &num))
		put_exit_errmsg_and_exit(str);
	exit(num & 255);
}

/*
 * builtin exit command
 * argv: ["exit", "0"]
 */
int	builtin_exit(char **argv)
{
	int	argv_len;

	argv_len = ptrarr_len((void **)argv);
	if (argv_len > 2)
		return (put_minish_err_msg_and_ret(1, "exit", "too many arguments"));
	else if (argv_len == 2)
		exit_atol(argv[1]);
	else
		exit(get_status());
	return (0);
}
