/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jtanaka <jtanaka@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/06 01:35:52 by jtanaka           #+#    #+#             */
/*   Updated: 2020/10/19 09:54:17 by jtanaka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memmove(void *dest, const void *src, size_t n)
{
	unsigned char	*src_bytes;
	unsigned char	*dest_bytes;
	int				idx;

	if (dest == src)
		return (dest);
	if (dest <= src)
		return (ft_memcpy(dest, src, n));
	src_bytes = (unsigned char*)src;
	dest_bytes = (unsigned char*)dest;
	idx = (int)n - 1;
	while (idx >= 0)
	{
		dest_bytes[idx] = src_bytes[idx];
		idx--;
	}
	return (dest);
}
