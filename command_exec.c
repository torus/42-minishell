#include "env.h"
#include "execution.h"

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
			put_err_msg_and_exit("error dup2()");
	}
	if (command->output_file_path)
	{
		fd = open(command->output_file_path, O_WRONLY | O_CREAT,
				S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
		if (fd == -1)
			put_err_msg_and_exit("error open()");
		if (dup2(fd, STDOUT_FILENO))
			put_err_msg_and_exit("error dup2()");
	}
	printf("----------------start exec----------------\n");
	ft_execvp((char *)command->exec_and_args[0], (char **)command->exec_and_args);
	return (ERROR);
}

int	command_execution(t_command_invocation *command)
{
	pid_t	pid;
	int		status;

	print_command(command);
	pid = fork();
	printf("processID: %d\n", getpid());
	if (pid == -1)
		return (put_err_msg_and_ret("failed fork()"));
	else if (pid == 0)
	{
		printf("parent ID: %d\n", getppid());
		/* Child process */
		spawn_child(command);
	}
	else
	{
		/* Parent process */
		printf("child ID: %d\n", pid);
		waitpid(pid, &status, 0);
		printf("child status: %d\n", status);
	}
	return (0);
}
