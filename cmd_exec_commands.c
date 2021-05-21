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
 * command: コマンド
 * stdoutfd: ビルトインコマンド終了後にSTDOUTを元に戻す用
 * stdinfd: ビルトインコマンド終了後にSTDINを元に戻す用
 */
static int	cmd_set_builtin_output_file(t_command_invocation *command, t_fd_list **fd_list, int *stdoutfd, int *stdinfd)
{
	int					fd;
	t_list				*current;
	t_cmd_redirection	*red;
	int					flag_open;
	char				*filepath;

	*stdoutfd = dup(STDOUT_FILENO);
	*stdinfd = dup(STDIN_FILENO);
	current = command->output_redirections;
	while (current)
	{
		red = (t_cmd_redirection *)current->content;
		filepath = expand_redirect_filepath((char *)red->filepath);
		if (!filepath)
			return (ERROR);
		flag_open = O_TRUNC * !red->is_append + O_APPEND * red->is_append;
		fd = open(filepath, O_WRONLY | O_CREAT | flag_open,
				S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
		free(filepath);
		if (fd == -1)
			return (put_err_msg_and_ret("error output file open()"));
		if (red->fd == *stdoutfd)
			*stdoutfd = dup(*stdoutfd);
		dup2(fd, red->fd);
		close(fd);
		fd_list_add_fd(fd_list, red->fd);
		current = current->next;
	}
	return (0);
}

/* ビルトインコマンドを実行する
 * パイプではなく単体(minishellと同じプロセス)
 * で実行されることを想定している
 */
int	cmd_exec_builtin(t_command_invocation *command)
{
	int				stdoutfd;
	int				stdinfd;
	int				status;
	t_builtin_cmd	*builtin_func;
	t_fd_list		*fd_lst;

	fd_lst = NULL;
	if (cmd_set_input_file(command) == ERROR
		|| cmd_set_builtin_output_file(command, &fd_lst, &stdoutfd, &stdinfd) == ERROR)
		return (put_err_msg_and_ret("error parent input/output file"));
	builtin_func = get_builtin_func((char *)command->exec_and_args[0]);
	status = builtin_func((char **)command->exec_and_args);
	set_status(status);
	fd_list_close(&fd_lst);
	if (dup2(stdoutfd, STDOUT_FILENO) == -1
		|| dup2(stdinfd, STDIN_FILENO) == -1)
		put_err_msg_and_exit("FDの復元に失敗");
	close(stdoutfd);
	close(stdinfd);
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
		status = WEXITSTATUS(status);
		command = command->piped_command;
	}
	set_status(status);
	return (status);
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
	if (!command->piped_command && command->exec_and_args
		&& is_builtin_command((char *)command->exec_and_args[0]))
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
