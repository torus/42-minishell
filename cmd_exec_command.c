#include "minishell.h"

/*
 * リダイレクション用に渡された文字列の環境変数展開を行う.
 *
 * 環境変数展開を行った結果, 文字列が0個や2つ以上の場合はエラーを表すNULLを返す.
 * エラーの場合, エラーメッセージをstderrに出力する.
 *
 * red_target: リダイレクション先の文字列
 *
 * return: 環境変数展開などが完了した文字列.
 *   展開した結果文字列が2つ以上の場合はエラーなのでNULLを返す.
 */
char	*expand_redirect_filepath(char *red_target)
{
	char	*expanded_str;
	char	**splitted_expanded_str;
	char	*errmsg;
	char	*filepath;

	expanded_str = expand_env_var(red_target);
	splitted_expanded_str = split_expanded_str(expanded_str);
	free(expanded_str);
	if (!splitted_expanded_str
		|| ptrarr_len((void **)splitted_expanded_str) != 1)
	{
		errmsg = ft_strjoin(red_target, ": ambiguous redirect\n");
		if (errmsg)
			put_err_msg(errmsg);
		free_ptrarr((void **)splitted_expanded_str);
		free(errmsg);
		return (NULL);
	}
	filepath = ft_strdup(splitted_expanded_str[0]);
	free_ptrarr((void **)splitted_expanded_str);
	return (filepath);
}

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
	char				*filepath;

	current = command->input_redirections;
	while (current)
	{
		red = (t_cmd_redirection *)current->content;
		filepath = expand_redirect_filepath((char *)red->filepath);
		if (!filepath)
			return (ERROR);
		fd = open(filepath, O_RDONLY);
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
	char				*filepath;

	current = command->output_redirections;
	while (current)
	{
		red = (t_cmd_redirection *)current->content;
		filepath = expand_redirect_filepath((char *)red->filepath);
		if (!filepath)
			return (ERROR);
		flag_open = O_TRUNC;
		if (red->is_append)
			flag_open = O_APPEND;
		fd = open(filepath, O_WRONLY | O_CREAT | flag_open,
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
	set_signal_handlers(SIG_DFL);
	if (pipe_prev_fd[1] >= 0)
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
