#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include "libft/libft.h"
#include "execution.h"
#include "env.h"
#include "minishell.h"

int	put_redir_errmsg_and_ret(int ret_value, int fd, char *msg)
{
	char	*fd_str;

	fd_str = ft_itoa(fd);
	if (!fd_str)
		put_minish_err_msg_and_ret(ERROR, "redirection", "ft_strjoin() failed");
	put_minish_err_msg(fd_str, msg);
	free(fd_str);
	return (ret_value);
}

/*
 * Expand variable of the string passed for redirection.
 * If error is occurred, put error message to stderr.
 *
 * red_target: The string of redirection node.
 *
 * Return: Filepath if variable expansion is succeeded.
 *   If the result of the environment variable expansion
 *     can be decomposed into two strings, an NULL is returned.
 */
char	*expand_redirect_filepath(char *red_target)
{
	char	*expanded_str;
	char	**splitted_expanded_str;
	char	*filepath;

	expanded_str = expand_env_var(red_target);
	splitted_expanded_str = split_expanded_str(expanded_str);
	free(expanded_str);
	if (!splitted_expanded_str
		|| ptrarr_len((void **)splitted_expanded_str) != 1)
	{
		put_minish_err_msg(red_target, "ambiguous redirect");
		free_ptrarr((void **)splitted_expanded_str);
		return (NULL);
	}
	filepath = ft_strdup(splitted_expanded_str[0]);
	free_ptrarr((void **)splitted_expanded_str);
	return (filepath);
}

int	open_file_for_redirect(t_cmd_redirection *red,
	int open_flags, mode_t open_mode)
{
	char	*filepath;
	int		fd;

	filepath = expand_redirect_filepath((char *)red->filepath);
	if (!filepath)
		return (ERROR);
	fd = open(filepath, open_flags, open_mode);
	if (fd == -1)
		put_minish_err_msg(filepath, strerror(errno));
	free(filepath);
	return (fd);
}

/*
 * open input file as stdin if command->input_file_path is exist
 *
 * command: command.
 *
 * return: return -1 if error has occurred, otherwise, return 0.
 */
int	cmd_set_input_file(t_command_invocation *command, int pipe_heredoc_fd[2])
{
	int					fd;
	t_list				*current;
	t_cmd_redirection	*red;

	current = command->input_redirections;
	close(pipe_heredoc_fd[1]);
	while (current)
	{
		red = (t_cmd_redirection *)current->content;
		if (!red->is_heredoc)
		{
			fd = open_file_for_redirect(red, O_RDONLY, 0);
			if (fd == ERROR)
				return (ERROR);
			if (dup2(fd, red->fd) == -1)
				return (put_redir_errmsg_and_ret(-1, red->fd, strerror(errno)));
		}
		else if (pipe_heredoc_fd[0] != -1
			&& dup2(pipe_heredoc_fd[0], red->fd) == -1)
			return (put_redir_errmsg_and_ret(-1, red->fd, strerror(errno)));
		current = current->next;
	}
	close(pipe_heredoc_fd[0]);
	return (0);
}

/*
 * open output file as stdout if command->output_file_path is exist
 *
 * command: command.
 *
 * return: return -1 if error has occurred, otherwise, return 0.
 */
int	cmd_set_output_file(t_command_invocation *command)
{
	int					fd;
	int					flag_open;
	t_cmd_redirection	*red;

	red = command->output_redirections;
	while (red)
	{
		flag_open = O_TRUNC * !red->is_append + O_APPEND * red->is_append;
		fd = open_file_for_redirect(red, O_WRONLY | O_CREAT | flag_open,
				S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
		if (fd == ERROR)
			return (ERROR);
		if (dup2(fd, red->fd) == -1)
			return (put_redir_errmsg_and_ret(ERROR,
					red->fd, strerror(errno)));
		red = red->next;
	}
	return (0);
}
