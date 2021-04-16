#include "env.h"
#include "execution.h"

/*
 * print t_command_invocation's attributes
 */
static void	cmd_print_command(t_command_invocation *command)
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
		cmd_print_command(command->piped_command);
}

/*
 * fork and execute commands.
 *
 * return: status of last command
 */
int	cmd_exec_commands(t_command_invocation *command)
{
	pid_t	pid;
	int		status;
	int		pipe_fd[2];
	int		pipe_prev_fd[2];
	t_list	*pid_lst;

	cmd_print_command(command);
	cmd_init_pipe_fd(pipe_prev_fd, STDIN_FILENO, -1);
	pid_lst = NULL;
	while (command)
	{
		if (!command->piped_command)
			pipe_fd[1] = STDOUT_FILENO;
		if (pipe(pipe_fd) == -1)
			return (put_err_msg_and_ret("error pipe()"));
		pid = fork();
		if (pid < 0)
			return (put_err_msg_and_ret("error fork()"));
		else if (pid > 0)
		{
			if (pipe_prev_fd[1] != -1)
				cmd_close_pipe(pipe_prev_fd);
			cmd_copy_pipe(pipe_prev_fd, pipe_fd);
			if (!cmd_lstadd_back_pid(&pid_lst, pid))
				return (put_err_msg_and_ret("error pid_lst add pid"));
		}
		else
			cmd_exec_command(command, pipe_prev_fd, pipe_fd);
		command = command->piped_command;
	}
	status = cmd_wait_pid_lst(pid_lst);
	ft_lstclear(&pid_lst, free);
	return (status);
}
