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
 * execute one command.
 *
 * command: command
 * pipe_prev_fd[2]: A pipe that connects the previous and current process.
 * pipe_fd[2]: A pipe that connects the current and next process.
 */
void	cmd_exec_cmd(t_command_invocation *command, int pipe_prev_fd[2], int pipe_fd[2])
{
	if (pipe_prev_fd)
	{
		close(pipe_prev_fd[1]);
		if (dup2(pipe_prev_fd[0], STDIN_FILENO) == -1)
			put_err_msg_and_exit("error child dup2()");
		close(pipe_prev_fd[0]);
	}
	if (pipe_fd)
	{
		close(pipe_fd[0]);
		if (dup2(pipe_fd[1], STDOUT_FILENO) == -1)
			put_err_msg_and_exit("error child dup2()");
		close(pipe_fd[1]);
	}
	if (cmd_set_input_file(command) == ERROR
		|| cmd_set_output_file(command) == ERROR)
		put_err_msg_and_exit("error input/output file");
	cmd_execvp((char *)command->exec_and_args[0], (char **) command->exec_and_args);
	put_err_msg_and_exit("error command execution");
}
