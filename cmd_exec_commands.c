#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "env.h"
#include "execution.h"
#include "builtin.h"

static int	cmd_connect_pipe(
	int pipe_prev_fd[2], int pipe_fd[2])
{
	if (pipe_prev_fd[1] != -1)
		cmd_close_pipe(pipe_prev_fd);
	cmd_copy_pipe(pipe_prev_fd, pipe_fd);
	return (0);
}

/*
 * Wait until all commands has finished and return exit code of last command.
 */
int	cmd_wait_commands(t_command_invocation *command)
{
	int	status;

	while (command)
	{
		if (command->pid > 0)
		{
			waitpid(command->pid, &status, 0);
			status = WEXITSTATUS(status);
		}
		command = command->piped_command;
	}
	set_status(status);
	return (status);
}

/*
 * Write to pipe_heredoc_fd[1]
 *   if last element of input_redirections is heredoc.
 */
static int	write_heredoc(t_fd_reds_list *in_fd_red_list)
{
	t_cmd_redirection	*red;

	while (in_fd_red_list)
	{
		red = in_fd_red_list->reds;
		while (red->next)
			red = red->next;
		close(in_fd_red_list->heredoc_pipe[0]);
		if (red->is_heredoc && red->filepath)
			write(in_fd_red_list->heredoc_pipe[1], red->filepath,
				ft_strlen(red->filepath));
		in_fd_red_list = in_fd_red_list->next;
	}
	return (0);
}

static int	cmd_exec_one_command(t_command_invocation *current_cmd,
	int	pipe_fd[2], int pipe_prev_fd[2])
{
	pid_t			pid;
	t_fd_reds_list	*in_fd_red_list;

	in_fd_red_list = reds2fd_reds_list(current_cmd->input_redirections);
	if (pipe(pipe_fd) || cmd_set_heredoc_pipe_fd(in_fd_red_list))
		return (put_err_msg_and_ret("error pipe()"));
	pid = fork();
	if (pid < 0)
		return (put_err_msg_and_ret("error fork()"));
	else if (pid == 0)
		cmd_exec_command(current_cmd, pipe_prev_fd, pipe_fd,
			in_fd_red_list);
	write_heredoc(in_fd_red_list);
	free_fd_reds_list(in_fd_red_list);
	current_cmd->pid = pid;
	if (cmd_connect_pipe(pipe_prev_fd, pipe_fd) != 0)
		return (put_err_msg_and_ret("error cmd_connect_pipe()"));
	return (0);
}

/*
 * fork and execute commands.
 *
 * prev_cmd <1--pipe_prev_fd--0> current_cmd <1--pipe_fd--0> next_cmd
 *
 * return: status of last command
 */
int	cmd_exec_commands(t_command_invocation *command)
{
	int						pipe_fd[2];
	int						pipe_prev_fd[2];
	t_command_invocation	*current_cmd;

	current_cmd = command;
	if (!command->piped_command && command->exec_and_args
		&& is_builtin_command((char *)command->exec_and_args[0]))
		return (cmd_exec_builtin(current_cmd));
	cmd_init_pipe_fd(pipe_prev_fd, STDIN_FILENO, -1);
	while (current_cmd)
	{
		if (cmd_exec_one_command(current_cmd, pipe_fd, pipe_prev_fd))
			break ;
		current_cmd = current_cmd->piped_command;
	}
	return (cmd_wait_commands(command));
}
