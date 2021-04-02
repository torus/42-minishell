/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstadd_back_new.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jtanaka <jtanaka@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/12 01:35:19 by jtanaka           #+#    #+#             */
/*   Updated: 2021/04/02 20:44:33 by jtanaka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_list	*ft_lstadd_back_new(t_list **lst, void *content)
{
	t_list	*new_list;

	new_list = ft_lstnew(content);
	if (!new_list)
		return (NULL);
	ft_lstadd_back(lst, new_list);
	return (*lst);
}
