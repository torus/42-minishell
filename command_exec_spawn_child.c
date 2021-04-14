#include "env.h"
#include "execution.h"

/*
 * open input file as stdin if command->input_file_path is exist
 *
 * command: command.
 *
 * return: return -1 if error has occurred, otherwise, return 0.
 */
int	cmd_set_input_file(t_command_invocation *command)
{
	int	fd;

	if (command->input_file_path)
	{
		fd = open(command->input_file_path, O_RDONLY);
		if (fd == -1)
			return (put_err_msg_and_ret("error open()"));
		if (dup2(fd, STDIN_FILENO) == -1)
			return (put_err_msg_and_ret("error dup2(fd, STDIN_NO)"));
	}
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
	int	fd;

	if (command->output_file_path)
	{
		fd = open(command->output_file_path, O_WRONLY | O_CREAT,
				S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
		if (fd == -1)
			return (put_err_msg_and_ret("error open()"));
		if (dup2(fd, STDOUT_FILENO) == -1)
			return (put_err_msg_and_ret("error dup2(fd, STDOUT_NO)"));
	}
	return (0);
}

/*
 * exec command in child process.
 *
 * command: information of execution command.
 *
 * return: If exec command successful, no value will be returned.
 *         This function returning value means exec or other function is failed.
 */
int	cmd_spawn_child(t_command_invocation *command)
{
	int		pipe_fd[2];
	int		pipe_prev_fd[2];
	pid_t	pid;

	pipe_prev_fd[0] = STDIN_FILENO;
	pipe_prev_fd[1] = -1;
	while (command->piped_command)
	{
		if (pipe(pipe_fd) == -1)
			return (put_err_msg_and_ret("error pipe()"));
		// fd_in = pipe_fd[0];
		// fd_out = pipe_fd[1];
		pid = fork();
		if (pid < 0)
			return (put_err_msg_and_ret("error fork()"));
		else if (pid > 0)
		{
			// 親プロセス
			pipe_prev_fd[0] = pipe_fd[0];
			pipe_prev_fd[1] = pipe_fd[1];
		}
		else
		{
			// パイプを繋げて受信できるようにする
			close(pipe_prev_fd[1]);
			if (dup2(pipe_prev_fd[0], STDIN_FILENO) == -1)
				return (put_err_msg_and_ret("error child dup2()"));
			close(pipe_fd[1]);
			if (dup2(pipe_fd[0], STDOUT_FILENO) == -1)
				return (put_err_msg_and_ret("error child dup2()"));
			// 子プロセス
			if (cmd_set_input_file(command) == ERROR
				|| cmd_set_output_file(command) == ERROR)
				return (put_err_msg_and_ret("error input/output file"));
			cmd_execvp((char *)command->exec_and_args[0], (char **) command->exec_and_args);
		}
		command = command->piped_command;
	}
	// パイプを繋げて受信できるようにしてexecvp
	close(pipe_prev_fd[1]);
	if (dup2(pipe_prev_fd[0], STDIN_FILENO) == -1)
		return (put_err_msg_and_ret("error child dup2()"));
	if (cmd_set_input_file(command) == ERROR || cmd_set_output_file(command) == ERROR)
		return (put_err_msg_and_ret("error input/output file"));
	cmd_execvp((char *)command->exec_and_args[0],
		(char **)command->exec_and_args);
	return (ERROR);
}
