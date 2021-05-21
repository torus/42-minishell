#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "env.h"
#include "execution.h"
#include "builtin.h"

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
