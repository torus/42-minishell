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
 * 最初と中間(最後以外)のコマンドを実行する
 *
 * command: コマンド
 * pipe_prev_fd[2]: 前のプロセスとのパイプ
 * pipe_fd[2]: 次のプロセスとのパイプ
 */
void	cmd_exec_cmd(t_command_invocation *command, int pipe_prev_fd[2], int pipe_fd[2])
{
	// パイプを繋げて受信できるようにする
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

int	cmd_wait_pid_lst(t_list *lst)
{
	int	status;

	while (lst)
	{
		waitpid(*((int *)lst->content), &status, 0);
		lst = lst->next;
	}
	return (status);
}

/*
 * exec command in child process.
 *
 * command: information of execution command.
 *
 * return: If exec command successful, no value will be returned.
 *         This function returning value means exec or other function is failed.
 */
int	cmd_spawn_child(t_command_invocation *command)
{
	int		pipe_fd[2];
	int		pipe_prev_fd[2];
	pid_t	pid;
	t_list	*lst;

	pipe_prev_fd[0] = STDIN_FILENO;
	pipe_prev_fd[1] = -1;
	lst = NULL;
	while (command->piped_command)
	{
		if (pipe(pipe_fd) == -1)
			return (put_err_msg_and_ret("error pipe()"));
		pid = fork();
		if (pid < 0)
			return (put_err_msg_and_ret("error fork()"));
		else if (pid > 0)
		{
			// 各pipeは親・子プロセス全体で入り口・出口を1つずつにしないといけない
			// 最初のコマンドでは前のプロセスとのパイプはないのでcloseしない
			if (pipe_prev_fd[0] != STDIN_FILENO)
				cmd_close_pipe(pipe_prev_fd);
			// 親プロセス
			cmd_copy_pipe(pipe_prev_fd, pipe_fd);
			if (!cmd_lstadd_back_pid(&lst, pid))
				return (put_err_msg_and_ret("error lst add pid"));
		}
		else
			cmd_exec_cmd(command, pipe_prev_fd, pipe_fd);
		command = command->piped_command;
	}
	// 最後のコマンド以外のプロセスが終了するのを待つ
	cmd_wait_pid_lst(lst);
	ft_lstclear(&lst, free);
	// パイプを繋げて受信できるようにしてexecvp
	cmd_exec_cmd(command, pipe_prev_fd, NULL);
	return (ERROR);
}
