#include "env.h"
#include "execution.h"
#include <string.h>
#include <errno.h>

/*
 * print t_command_invocation's attributes
 */
static void	print_command(t_command_invocation *command)
{
	size_t	i;

	printf("output_file_path: %s\n", command->output_file_path);
	printf("input_file_path: %s\n", command->input_file_path);
	printf("command: ");
	i = 0;
	while (command->exec_and_args[i])
		printf("%s ", command->exec_and_args[i++]);
	printf("\n");
	printf("piped_command: %p\n", command->piped_command);
	if (command->piped_command)
		print_command(command->piped_command);
}

static int	set_input_file(t_command_invocation *command)
{
	int	fd;

	if (command->input_file_path)
	{
		fd = open(command->input_file_path, O_RDONLY);
		if (fd == -1)
			put_err_msg_and_ret("error open()");
		if (dup2(fd, STDIN_FILENO) == -1)
			put_err_msg_and_ret("error dup2(fd, STDIN_NO)");
	}
	return (0);
}

static int	set_output_file(t_command_invocation *command)
{
	int	fd;

	if (command->output_file_path)
	{
		fd = open(command->output_file_path, O_WRONLY | O_CREAT,
				S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
		if (fd == -1)
			put_err_msg_and_ret("error open()");
		if (dup2(fd, STDOUT_FILENO) == -1)
			put_err_msg_and_ret("error dup2(fd, STDOUT_NO)");
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
static int	spawn_child(t_command_invocation *command)
{
	int		pipe_fd[2];
	pid_t	pid;

	if (command->piped_command && pipe(pipe_fd) == -1)
		return (put_err_msg_and_ret("error pipe()"));
	if (set_input_file(command) == ERROR)
		return (put_err_msg_and_ret("error set_input_file(command)"));
	if (set_output_file(command) == ERROR)
		return (put_err_msg_and_ret("error set_output_file(command)"));
	if (command->piped_command)
	{
		pid = fork();
		if (pid == -1)
			return (put_err_msg_and_ret("error fork()"));
		else if (pid > 0)
		{
			close(pipe_fd[0]);
			if (!command->output_file_path
				&& dup2(pipe_fd[1], STDOUT_FILENO) == -1)
				put_err_msg_and_exit("error dup2(pipe_fd[1], STDOUT_NO)");
		}
		else
		{
			close(pipe_fd[1]);
			if (!command->input_file_path
				&& dup2(pipe_fd[0], STDIN_FILENO) == -1)
				return (put_err_msg_and_ret("error dup2(pipe_fd[0], STDIN_FILENO)"));
			spawn_child(command->piped_command);
		}
	}
	ft_execvp((char *)command->exec_and_args[0],
		(char **)command->exec_and_args);
	return (ERROR);
}

/*
 * fork and execute command.
 */
int	command_execution(t_command_invocation *command)
{
	pid_t	pid;
	int		status;

	print_command(command);
	pid = fork();
	if (pid == -1)
		return (put_err_msg_and_ret("error fork()"));
	else if (pid > 0)
	{
		waitpid(pid, &status, 0);
	}
	else
	{
		status = spawn_child(command);
	}
	return (status);
}
