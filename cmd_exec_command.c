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
	int					fd;
	t_list				*current;
	t_cmd_redirection	*red;

	current = command->input_redirections;
	while (current)
	{
		red = (t_cmd_redirection *)current->content;
		fd = open(red->filepath, O_RDONLY);
		if (fd == -1)
			return (put_err_msg_and_ret("error open()"));
		if (!current->next && dup2(fd, STDIN_FILENO) == -1)
			return (put_err_msg_and_ret("error dup2(fd, STDIN_NO)"));
		current = current->next;
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
	int					fd;
	t_list				*current;
	t_cmd_redirection	*red;
	int					flag_open;

	current = command->output_redirections;
	while (current)
	{
		red = (t_cmd_redirection *)current->content;
		flag_open = 0;
		if (red->is_append)
			flag_open |= O_APPEND;
		else
			flag_open |= O_TRUNC;
		fd = open(red->filepath, O_WRONLY | O_CREAT | flag_open,
				S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
		if (fd == -1)
			return (put_err_msg_and_ret("error open()"));
		if (!current->next && dup2(fd, STDOUT_FILENO) == -1)
			return (put_err_msg_and_ret("error dup2(fd, STDOUT_NO)"));
		current = current->next;
	}
	return (0);
}

/*
 * Execute a command.
 * This function is supposed to be called in child process.
 *
 * command: command
 * pipe_prev_fd[2]: A pipe that connects the previous and current process.
 * pipe_fd[2]: A pipe that connects the current and next process.
 */
void	cmd_exec_command(t_command_invocation *command,
	int pipe_prev_fd[2], int pipe_fd[2])
{
	if (pipe_prev_fd[1] >= 0)  // 最初のコマンド時, pipe_prev_fd[1]は-1
	{
		close(pipe_prev_fd[1]);
		if (dup2(pipe_prev_fd[0], STDIN_FILENO) == -1)
			put_err_msg_and_exit("error child dup2()");
		close(pipe_prev_fd[0]);
	}
	if (command->piped_command)
	{
		close(pipe_fd[0]);
		if (dup2(pipe_fd[1], STDOUT_FILENO) == -1)
			put_err_msg_and_exit("error child dup2()");
		close(pipe_fd[1]);
	}
	if (cmd_set_input_file(command) == ERROR
		|| cmd_set_output_file(command) == ERROR)
		put_err_msg_and_exit("error input/output file");
	cmd_execvp((char *)command->exec_and_args[0],
		(char **) command->exec_and_args);
	put_err_msg_and_exit("error command execution");
}
