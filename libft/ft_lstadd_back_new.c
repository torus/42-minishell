/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstadd_back_new.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jtanaka <jtanaka@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/12 01:35:19 by jtanaka           #+#    #+#             */
/*   Updated: 2021/03/12 03:17:11 by jtanaka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_list	*ft_lstadd_back_new(t_list **lst, void *content)
{
	t_list *new_list;

	if (!(new_list = ft_lstnew(content)))
		return (NULL);
	ft_lstadd_back(lst, new_list);
	return (*lst);
}
