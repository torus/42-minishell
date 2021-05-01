#include "env.h"
#include "execution.h"

static int	cmd_connect_pipe(
	int pipe_prev_fd[2], int pipe_fd[2], t_list **pid_lst, int pid)
{
	if (pipe_prev_fd[1] != -1)
		cmd_close_pipe(pipe_prev_fd);
	cmd_copy_pipe(pipe_prev_fd, pipe_fd);
	if (!cmd_lstadd_back_pid(pid_lst, pid))
		return (ERROR);
	return (0);
}

/*
 * fork and execute commands.
 *
 * return: status of last command
 */
int	cmd_exec_commands(t_command_invocation *command)
{
	pid_t	pid;
	int		pipe_fd[2];
	int		pipe_prev_fd[2];
	t_list	*pid_lst;

	cmd_init_pipe_fd(pipe_prev_fd, STDIN_FILENO, -1);
	pid_lst = NULL;
	while (command)
	{
		if (pipe(pipe_fd) == -1)
			return (put_err_msg_and_ret("error pipe()"));
		// TODO: ビルドインコマンドだった場合はforkせずに親プロセスで実行
		pid = fork();
		if (pid < 0)
			return (put_err_msg_and_ret("error fork()"));
		else if (pid == 0)
			cmd_exec_command(command, pipe_prev_fd, pipe_fd);
		else
			if (cmd_connect_pipe(pipe_prev_fd, pipe_fd, &pid_lst, pid) != 0)
				return (put_err_msg_and_ret("error cmd_connect_pipe()"));
		command = command->piped_command;
	}
	return (cmd_wait_pid_lst(pid_lst));
}
