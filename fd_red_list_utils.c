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

static void	addback_red_in_fd_red_list(t_fd_red_list *fd_red_list, t_cmd_redirection *red)
{
	t_fd_red_list	*new;

	new = malloc(sizeof(t_cmd_redirection));
	check_malloc_has_succeeded("fd_red_list", new);
	new->fd = red->fd;
	new->red = red;
	cmd_init_pipe_fd(new->pipe, -1, -1);
	new->next = NULL;
	while (fd_red_list->next)
		fd_red_list = fd_red_list->next;
	fd_red_list->next = new;
}

t_fd_red_list	*red_list2fd_red_list(t_list *red_list)
{
	t_fd_red_list		*fd_red_list;
	t_cmd_redirection	*red;

	fd_red_list = NULL;
	while (red_list)
	{
		red = (t_cmd_redirection *)red_list->content;
		if (!get_fd_red_from_list(fd_red_list, red->fd))
			addback_red_in_fd_red_list(fd_red_list, red);
		else
			get_fd_red_from_list(fd_red_list, red->fd)->red = red;
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
