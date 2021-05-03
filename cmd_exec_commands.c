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

/* ビルトインコマンドを実行する
 * パイプではなく単体で実行されることを想定している
 */
int	cmd_exec_builtin(t_command_invocation *command)
{
	int	status;

	if (cmd_set_input_file(command) == ERROR
		|| cmd_set_output_file(command) == ERROR)
		return (put_err_msg_and_ret("error parent input/output file"));
	status = get_builtin_func((char *)command->exec_and_args[0])((char **)command->exec_and_args);
	set_status(status);
	return (status);
}

/* コマンドが実行完了するまでwaitする.
 */
int	cmd_wait_commands(t_command_invocation *command)
{
	int	status;

	while (command)
	{
		waitpid(command->pid, &status, 0);
		// 最後のコマンドの場合はステータスをセット
		if (!command->piped_command)
			set_status(status);
		command = command->piped_command;
	}
	return (0);
}

/*
 * fork and execute commands.
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
	// ビルトインコマンドで且つパイプじゃなかったら親プロセスで実行
	if (!command->piped_command && is_builtin_command((char *)command->exec_and_args[0]))
		return (cmd_exec_builtin(current_cmd));
	cmd_init_pipe_fd(pipe_prev_fd, STDIN_FILENO, -1);
	while (current_cmd)
	{
		if (pipe(pipe_fd) == -1)
			return (put_err_msg_and_ret("error pipe()"));
			pid = fork();
		if (pid < 0)
			return (put_err_msg_and_ret("error fork()"));
		else if (pid == 0)
			cmd_exec_command(current_cmd, pipe_prev_fd, pipe_fd);
		current_cmd->pid = pid;
		if (cmd_connect_pipe(pipe_prev_fd, pipe_fd) != 0)
			return (put_err_msg_and_ret("error cmd_connect_pipe()"));
		current_cmd = current_cmd->piped_command;
	}
	return (cmd_wait_commands(command));
}
