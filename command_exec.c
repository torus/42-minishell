#include "env.h"
#include "execution.h"

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
