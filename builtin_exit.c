#include "minishell.h"
#include "builtin.h"

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
		exit(ft_atoi(argv[1]));
	else
		exit(0);
	return (0);
}
