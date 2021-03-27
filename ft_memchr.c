/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jtanaka <jtanaka@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/06 01:56:41 by jtanaka           #+#    #+#             */
/*   Updated: 2020/10/22 17:51:18 by jtanaka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memchr(const void *s, int c, size_t n)
{
	unsigned char	*mem;
	size_t			idx;

	mem = (unsigned char*)s;
	idx = 0;
	while (idx < n)
	{
		if (mem[idx] == (unsigned char)c)
			return (mem + idx);
		idx++;
	}
	return (NULL);
}
