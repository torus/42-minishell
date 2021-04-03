/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jtanaka <jtanaka@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/25 02:22:55 by jtanaka           #+#    #+#             */
/*   Updated: 2021/04/02 18:53:09 by jtanaka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

t_gnl_list	*create_fd_elem(t_gnl_list **lst, int fd)
{
	t_gnl_list	*new;

	if (!lst)
		return (NULL);
	new = malloc(sizeof(t_gnl_list));
	if (!new)
		return (NULL);
	new->fd = fd;
	new->save = NULL;
	if (!(*lst))
	{
		*lst = new;
		new->next = NULL;
	}
	else
	{
		new->next = *lst;
		*lst = new;
	}
	return (new);
}

void	remove_fd_elem(t_gnl_list **list_head, t_gnl_list *target_elem)
{
	t_gnl_list	*tmp;

	free(target_elem->save);
	if (*list_head == target_elem)
		*list_head = target_elem->next;
	else
	{
		tmp = *list_head;
		while (tmp->next != target_elem)
			tmp = tmp->next;
		tmp->next = target_elem->next;
	}
	free(target_elem);
}
