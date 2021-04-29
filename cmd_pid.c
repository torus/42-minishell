#include "execution.h"
#include "env.h"

/*
 * Add pid in list.
 *
 * lst: list.
 * pid: process ID to be added
 *
 * return: NULL if malloc is failed.
 *         Otherwise, return new list pointer.
 */
t_list	*cmd_lstadd_back_pid(t_list **lst, int pid)
{
	int	*pidptr;

	pidptr = malloc(sizeof(int));
	if (!pidptr)
		return (NULL);
	*pidptr = pid;
	return (ft_lstadd_back_new(lst, (void *)pidptr));
}

/*
 * Wait processes in list and clean list.
 *
 * lst: List that has process id as content.
 *
 * return: status of last process.
 */
int	cmd_wait_pid_lst(t_list *lst)
{
	int		status;
	t_list	*current_lst;

	current_lst = lst;
	while (current_lst)
	{
		waitpid(*((int *)current_lst->content), &status, 0);
		current_lst = current_lst->next;
	}
	set_status(status);  // $? には最後のコマンドのステータスを格納する
	ft_lstclear(&lst, free);
	return (status);
}
