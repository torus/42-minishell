#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "env.h"
#include "execution.h"
#include "builtin.h"

static int	cmd_connect_pipe(
	int pipe_prev_fd[2], int pipe_fd[2])
{
	if (pipe_prev_fd[1] != -1)
		cmd_close_pipe(pipe_prev_fd);
	cmd_copy_pipe(pipe_prev_fd, pipe_fd);
	return (0);
}

/*
 * Wait until all commands has finished and return exit code of last command.
 */
int	cmd_wait_commands(t_command_invocation *command)
{
	int	status;

	while (command)
	{
		waitpid(command->pid, &status, 0);
		status = WEXITSTATUS(status);
		command = command->piped_command;
	}
	set_status(status);
	return (status);
}

#include <stdio.h>
/*
 * input_redirectionsの最後がheredocの場合, pipe_prev_fd(pipe_fd[1]) に書き込みを行う
 * pipe_fd: commandプロセスに対するpipe_fd
 */
int	write_heredoc(t_command_invocation *command, int pipe_prev_fd[2])
{
	t_list				*current;
	t_cmd_redirection	*red;

	if (!command->input_redirections)
		return (0);
	current = command->input_redirections;
	while (current->next)
		current = current->next;
	red = (t_cmd_redirection *)current->content;
	if (!red->is_heredoc)
		return (0);
	printf("----- write(%d) -----\n", pipe_prev_fd[1]);
	printf("file_path: \n|%s|\n", red->filepath);
	write(pipe_prev_fd[1], red->filepath, ft_strlen(red->filepath));
	return (0);
}

/*
 * fork and execute commands.
 *
 * prev_cmd <1--pipe_prev_fd--0> current_cmd <1--pipe_fd--0> next_cmd
 *
 * return: status of last command
 */
int	cmd_exec_commands(t_command_invocation *command)
{
	pid_t					pid;
	int						pipe_fd[2];
	int						pipe_prev_fd[2];
	t_command_invocation	*current_cmd;

	current_cmd = command;
	if (!command->piped_command && command->exec_and_args
		&& is_builtin_command((char *)command->exec_and_args[0]))
		return (cmd_exec_builtin(current_cmd));
	cmd_init_pipe_fd(command, pipe_prev_fd);
	while (current_cmd)
	{
		if (pipe(pipe_fd) == -1)
			return (put_err_msg_and_ret("error pipe()"));
		pid = fork();
		if (pid < 0)
			return (put_err_msg_and_ret("error fork()"));
		else if (pid == 0)
			cmd_exec_command(current_cmd, pipe_prev_fd, pipe_fd);
		write_heredoc(current_cmd, pipe_prev_fd);
		current_cmd->pid = pid;
		if (cmd_connect_pipe(pipe_prev_fd, pipe_fd) != 0)
			return (put_err_msg_and_ret("error cmd_connect_pipe()"));
		current_cmd = current_cmd->piped_command;
	}
	return (cmd_wait_commands(command));
}
