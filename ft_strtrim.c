/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jtanaka <jtanaka@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/11 03:19:24 by jtanaka           #+#    #+#             */
/*   Updated: 2020/10/21 23:55:32 by jtanaka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char		*ft_strtrim(char const *s1, char const *set)
{
	int				idx;
	int				start_idx;
	int				last_idx;

	if (!s1 || !*s1)
		return (ft_calloc(sizeof(char), 1));
	start_idx = 0;
	idx = 0;
	while (ft_strchr((char*)set, s1[idx]))
		start_idx = ++idx;
	last_idx = ft_strlen(s1) - 1;
	idx = ft_strlen(s1) - 1;
	while (ft_strchr((char*)set, s1[idx]))
		last_idx = --idx;
	if (start_idx >= last_idx)
		return (ft_calloc(sizeof(char), 1));
	return (ft_substr(s1, start_idx, last_idx - start_idx + 1));
}
