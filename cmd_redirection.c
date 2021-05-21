#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "libft/libft.h"
#include "execution.h"
#include "env.h"
#include "minishell.h"

t_fd_list	*fd_list_add_fd(t_fd_list **lst, int fd)
{
	t_fd_list	*new_lst;
	t_fd_list	*tmp;

	if (!lst)
		return (NULL);
	new_lst = ft_calloc(1, sizeof(t_fd_list));
	new_lst->fd = fd;
	if (!new_lst)
		return (NULL);
	if (!*lst)
		*lst = new_lst;
	else
	{
		tmp = *lst;
		while (tmp->next)
			tmp = tmp->next;
		tmp->next = new_lst;
	}
	return (new_lst);
}

#include <stdio.h>
void		fd_list_close(t_fd_list **lst)
{
	t_fd_list	*current;
	t_fd_list	*tmp;

	if (!lst)
		return ;
	current = *lst;
	while (current)
	{
		close(current->fd);
		tmp = current;
		current = current->next;
		free(tmp);
	}
	*lst = NULL;
}

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
		free(filepath);
		if (fd == -1)
			return (put_err_msg_and_ret("error input file open()"));
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
	char				*filepath;

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
		if (dup2(fd, red->fd) == -1)
			return (put_err_msg_and_ret("error dup2(fd, STDOUT_NO)"));
		current = current->next;
	}
	return (0);
}
