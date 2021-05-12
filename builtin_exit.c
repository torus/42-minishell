#include "minishell.h"
#include "builtin.h"

static void	put_exit_errmsg_and_exit(char *exit_status)
{
	char	*tmp;

	tmp = ft_strjoin(exit_status, ": numeric argument required");
	if (!tmp)
		put_minish_err_msg_and_exit(2, "exit", "ft_strjoin() failed");
	put_minish_err_msg_and_exit(2, "exit", tmp);
}

static int	exit_atoi(char *str)
{
	int	num;
	int	i;

	i = 0;
	num = 0;
	while (str[i])
	{
		if (!ft_isdigit(str[i])
			|| num > (INT_MAX - (str[i] - '0')) / 10)
			put_exit_errmsg_and_exit(str);
		num = num * 10 + (str[i] - '0');
		i++;
	}
	exit(num);
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
		exit_atoi(argv[1]);
	else
		exit(get_status());
	return (0);
}
