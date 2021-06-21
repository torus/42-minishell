#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
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

void	fd_list_close(t_fd_list **lst)
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
