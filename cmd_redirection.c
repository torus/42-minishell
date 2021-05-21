#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include "libft/libft.h"
#include "execution.h"
#include "env.h"
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

int	open_file_for_redirect(t_cmd_redirection *red,
	int open_flags, mode_t open_mode)
{
	char	*filepath;
	int		fd;

	filepath = expand_redirect_filepath((char *)red->filepath);
	if (!filepath)
		return (ERROR);
	fd = open(filepath, open_flags, open_mode);
	free(filepath);
	return (fd);
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

	current = command->input_redirections;
	while (current)
	{
		red = (t_cmd_redirection *)current->content;
		fd = open_file_for_redirect(red, O_RDONLY, 0);
		if (fd == -1)
			return (put_minish_err_msg_and_ret(-1,
					"in_redirect", strerror(errno)));
		if (dup2(fd, red->fd) == -1)
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
	int					flag_open;
	t_cmd_redirection	*red;

	current = command->output_redirections;
	while (current)
	{
		red = (t_cmd_redirection *)current->content;
		flag_open = O_TRUNC * !red->is_append + O_APPEND * red->is_append;
		fd = open_file_for_redirect(red, O_WRONLY | O_CREAT | flag_open,
				S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
		if (fd == -1)
			return (put_minish_err_msg_and_ret(-1,
					"out_redirect", strerror(errno)));
		if (dup2(fd, red->fd) == -1)
			return (put_err_msg_and_ret("error dup2(fd, STDIN_NO)"));
		current = current->next;
	}
	return (0);
}
