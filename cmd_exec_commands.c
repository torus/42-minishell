#include "env.h"
#include "execution.h"
#include "builtin.h"

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

/* ビルトインコマンドを実行する
 */
int	cmd_exec_builtin(t_command_invocation *command,
	int pipe_prev_fd[2], int pipe_fd[2])
{
	int	stdoutfd;
	int	stdinfd;
	int	status;

	stdoutfd = dup(STDOUT_FILENO);
	stdinfd = dup(STDIN_FILENO);
	if (pipe_prev_fd[1] >= 0)
	{
		close(pipe_prev_fd[1]);
		if (dup2(pipe_prev_fd[0], STDIN_FILENO) == -1)
			put_err_msg_and_exit("error parent dup2()");
		close(pipe_prev_fd[0]);
	}
	if (command->piped_command)
	{
		close(pipe_fd[0]);
		if (dup2(pipe_fd[1], STDOUT_FILENO) == -1)
			put_err_msg_and_exit("error parent dup2()");
		close(pipe_fd[1]);
	}
	if (cmd_set_input_file(command) == ERROR
		|| cmd_set_output_file(command) == ERROR)
		return (put_err_msg_and_ret("error parent input/output file"));
	status = get_builtin_func((char *)command->exec_and_args[0])((char **)command->exec_and_args);
	if (dup2(stdoutfd, STDOUT_FILENO) == -1
		|| dup2(stdinfd, STDIN_FILENO) == -1)
		put_err_msg_and_exit("error parent revert stdio back");
	return (status);
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
		// ビルドインコマンドだった場合はforkせずに親プロセスで実行
		if (is_builtin_command((char *)command->exec_and_args[0]))
			cmd_exec_builtin(command, pipe_prev_fd, pipe_fd);
		else
		{
			pid = fork();
			if (pid < 0)
				return (put_err_msg_and_ret("error fork()"));
			else if (pid == 0)
				cmd_exec_command(command, pipe_prev_fd, pipe_fd);
		}
		if (cmd_connect_pipe(pipe_prev_fd, pipe_fd, &pid_lst, pid) != 0)
			return (put_err_msg_and_ret("error cmd_connect_pipe()"));
		command = command->piped_command;
	}
	return (cmd_wait_pid_lst(pid_lst));
}
