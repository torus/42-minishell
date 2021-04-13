#include "env.h"
#include "execution.h"

int	set_input_file(t_command_invocation *command)
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

int	set_output_file(t_command_invocation *command)
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

int	pipe_process(t_command_invocation *command, int pipe_fd[2])
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
		return (put_err_msg_and_ret("error fork()"));
	else if (pid > 0)
	{
		close(pipe_fd[0]);
		if (!command->output_file_path
			&& dup2(pipe_fd[1], STDOUT_FILENO) == -1)
			return (put_err_msg_and_ret("error dup2(pipe_fd[1], STDOUT_NO)"));
	}
	else
	{
		close(pipe_fd[1]);
		if (!command->input_file_path
			&& dup2(pipe_fd[0], STDIN_FILENO) == -1)
			return (put_err_msg_and_ret(
					"error dup2(pipe_fd[0], STDIN_FILENO)"));
		spawn_child(command->piped_command);
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
int	spawn_child(t_command_invocation *command)
{
	int		pipe_fd[2];

	if (command->piped_command && pipe(pipe_fd) == -1)
		return (put_err_msg_and_ret("error pipe()"));
	if (set_input_file(command) == ERROR || set_output_file(command) == ERROR)
		return (put_err_msg_and_ret("error input/output file"));
	if (command->piped_command && pipe_process(command, pipe_fd) == ERROR)
		return (put_err_msg_and_ret("error pipe process"));
	ft_execvp((char *)command->exec_and_args[0],
		(char **)command->exec_and_args);
	return (ERROR);
}
