#include "execution.h"
#include "minishell.h"
#include "utils.h"

static t_fd_red_list	*get_fd_red_from_list(t_fd_red_list *fd_red_list, int fd)
{
	while (fd_red_list)
	{
		if (fd_red_list->fd == fd)
			return (fd_red_list);
		fd_red_list = fd_red_list->next;
	}
	return (NULL);
}

static void	addback_new_fd_red2fd_red_list(t_fd_red_list *fd_red_list, t_cmd_redirection *red)
{
	t_fd_red_list	*new;

	new = malloc(sizeof(t_cmd_redirection));
	check_malloc_has_succeeded("fd_red_list", new);
	new->fd = red->fd;
	new->reds = red;
	cmd_init_pipe_fd(new->pipe, -1, -1);
	new->next = NULL;
	while (fd_red_list->next)
		fd_red_list = fd_red_list->next;
	fd_red_list->next = new;
}

t_fd_red_list	*reds2fd_red_list(t_cmd_redirection *reds)
{
	t_fd_red_list		*fd_red_list;

	fd_red_list = NULL;
	while (reds)
	{
		if (!get_fd_red_from_list(fd_red_list, reds->fd))
			addback_new_fd_red2fd_red_list(fd_red_list, reds);
		else
			cmd_redirection_add_back(get_fd_red_from_list(fd_red_list, reds->fd)->reds, reds);
		reds = reds->next;
	}
	return (fd_red_list);
}

void	free_fd_red_list(t_fd_red_list *fd_red_list)
{
	t_fd_red_list	*tmp;

	while (fd_red_list)
	{
		tmp = fd_red_list;
		fd_red_list = fd_red_list->next;
		free(tmp);
	}
}
