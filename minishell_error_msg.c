#include "minishell.h"

/*
 * Output error messages of the following format to stderr.
 * "minishell: {cmd_name}: {msg}\n"
 */
void	put_minish_err_msg(const char *cmd_name, const char *msg)
{
	write(STDERR_FILENO, "minishell: ", 11);
	if (cmd_name)
		write(STDERR_FILENO, cmd_name, ft_strlen(cmd_name));
	write(STDERR_FILENO, ": ", 2);
	if (msg)
		write(STDERR_FILENO, msg, ft_strlen(msg));
	write(STDERR_FILENO, "\n", 1);
}

/*
 * Output error messages of the following format to stderr and return ret_val.
 * "minishell: {cmd_name}: {msg}\n"
 */
int	put_minish_err_msg_and_ret(int ret_val,
	const char *cmd_name, const char *msg)
{
	put_minish_err_msg(cmd_name, msg);
	return (ret_val);
}

/*
 * Output error messages of the following format to stderr and exit(status).
 * "minishell: {cmd_name}: {msg}\n"
 */
void	put_minish_err_msg_and_exit(int status,
	const char *cmd_name, const char *msg)
{
	put_minish_err_msg(cmd_name, msg);
	exit(status);
}

void	check_malloc_has_succeeded(char *cmd_name, void *ptr)
{
	if (!ptr)
	{
		if (cmd_name)
			put_minish_err_msg_and_exit(1, cmd_name, "malloc() failed");
		else
			put_minish_err_msg_and_exit(1, "malloc", "malloc() failed");
	}
}
