#include "execution.h"

t_list	*cmd_lstadd_back_pid(t_list **lst, int pid)
{
	int	*pidptr;

	pidptr = malloc(sizeof(int));
	if (!pidptr)
		return (NULL);
	*pidptr = pid;
	return (ft_lstadd_back_new(lst, (void *)pidptr));
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

