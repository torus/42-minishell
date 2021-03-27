/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jtanaka <jtanaka@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/06 05:09:40 by jtanaka           #+#    #+#             */
/*   Updated: 2020/10/22 18:10:08 by jtanaka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char		*ft_strnstr(const char *big, const char *little, size_t len)
{
	size_t big_len;
	size_t little_len;
	size_t i;

	if (*little == '\0')
		return ((char*)big);
	big_len = ft_strlen(big);
	little_len = ft_strlen(little);
	i = 0;
	while (i + little_len <= len && i < big_len)
	{
		if (ft_strncmp(big + i, little, little_len) == 0)
			return ((char*)big + i);
		i++;
	}
	return (NULL);
}
