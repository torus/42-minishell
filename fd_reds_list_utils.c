#include "execution.h"
#include "minishell.h"
#include "utils.h"

static t_fd_reds_list	*get_fd_red_from_list(
	t_fd_reds_list *fd_red_list, int fd)
{
	while (fd_red_list)
	{
		if (fd_red_list->fd == fd)
			return (fd_red_list);
		fd_red_list = fd_red_list->next;
	}
	return (NULL);
}

static void	addback_new_fd_red2fd_red_list(
	t_fd_reds_list **fd_red_list, t_cmd_redirection *red)
{
	t_fd_reds_list	*tmp;
	t_fd_reds_list	*new_fd_red;

	new_fd_red = malloc(sizeof(t_fd_reds_list));
	check_malloc_has_succeeded("fd_red_list", new_fd_red);
	new_fd_red->fd = red->fd;
	cmd_init_pipe_fd(new_fd_red->heredoc_pipe, -1, -1);
	red->next = NULL;
	new_fd_red->reds = NULL;
	cmd_redirection_add_back(&new_fd_red->reds, red);
	new_fd_red->next = NULL;
	if (!*fd_red_list)
		*fd_red_list = new_fd_red;
	else
	{
		tmp = *fd_red_list;
		while (tmp->next)
			tmp = tmp->next;
		tmp->next = new_fd_red;
	}
}

t_fd_reds_list	*reds2fd_reds_list(t_cmd_redirection *reds)
{
	t_fd_reds_list		*fd_red_list;
	t_fd_reds_list		*fd_red_element;
	t_cmd_redirection	*new_red;

	fd_red_list = NULL;
	while (reds)
	{
		fd_red_element = get_fd_red_from_list(fd_red_list, reds->fd);
		new_red = cmd_copy_redirection(reds);
		check_malloc_has_succeeded("addback_new_fd_red2fd_red_list", new_red);
		if (!fd_red_element)
			addback_new_fd_red2fd_red_list(&fd_red_list, new_red);
		else
			cmd_redirection_add_back(&fd_red_element->reds, new_red);
		reds = reds->next;
	}
	return (fd_red_list);
}

void	free_fd_red_list(t_fd_reds_list *fd_red_list)
{
	t_fd_reds_list	*tmp;

	while (fd_red_list)
	{
		tmp = fd_red_list;
		fd_red_list = fd_red_list->next;
		cmd_close_pipe(tmp->heredoc_pipe);
		cmd_free_redirections(tmp->reds);
		free(tmp);
	}
}
