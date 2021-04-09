#include "env.h"
#include "execution.h"
#include <string.h>
#include <errno.h>

static void	print_command(t_command_invocation *command)
{
	printf("output_file_path: %s\n", command->output_file_path);
	printf("input_file_path: %s\n", command->input_file_path);
	printf("command: ");
	for (int i = 0; command->exec_and_args[i]; i++)
		printf("%s ", command->exec_and_args[i]);
	printf("\n");
	printf("piped_command: %p\n", command->piped_command);
	if (command->piped_command)
		print_command(command->piped_command);
}

static int	spawn_child(t_command_invocation *command)
{
	int	fd;

	if (command->input_file_path)
	{
		fd = open(command->input_file_path, O_RDONLY);
		if (fd == -1)
			put_err_msg_and_exit("error open()");
		if (dup2(fd, STDIN_FILENO) == -1)
			put_err_msg_and_exit("error dup2(fd, STDIN_NO)");
	}
	if (command->output_file_path)
	{
		fd = open(command->output_file_path, O_WRONLY | O_CREAT,
				S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
		if (fd == -1)
			put_err_msg_and_exit("error open()");
		if (dup2(fd, STDOUT_FILENO) == -1)
			put_err_msg_and_exit("error dup2(fd, STDOUT_NO)");
	}
	ft_execvp((char *)command->exec_and_args[0], (char **)command->exec_and_args);
	return (ERROR);
}

int	command_execution(t_command_invocation *command)
{
	pid_t	pid;
	int		status;

	print_command(command);
	pid = fork();
	if (pid == -1)
		return (put_err_msg_and_ret("error fork()"));
	else if (pid == 0)
	{
		/* Child process */
		status = spawn_child(command);
	}
	else
	{
		/* Parent process */
		waitpid(pid, &status, 0);
	}
	return (status);
}
