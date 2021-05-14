#include "minishell.h"

/*
 * 以下の形式のエラーメッセージをstderrに出力する
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
 * 以下の形式のエラーメッセージをstderrに出力して return(ret_val);
 * "minishell: {cmd_name}: {msg}\n"
 */
int	put_minish_err_msg_and_ret(int ret_val,
	const char *cmd_name, const char *msg)
{
	put_minish_err_msg(cmd_name, msg);
	return (ret_val);
}

/*
 * 以下の形式のエラーメッセージをstderrに出力して exit(status);
 * "minishell: {cmd_name}: {msg}\n"
 */
void	put_minish_err_msg_and_exit(int status,
	const char *cmd_name, const char *msg)
{
	put_minish_err_msg(cmd_name, msg);
	exit(status);
}
