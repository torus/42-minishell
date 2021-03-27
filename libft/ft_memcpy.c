/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jtanaka <jtanaka@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/06 00:54:12 by jtanaka           #+#    #+#             */
/*   Updated: 2020/10/16 01:22:01 by jtanaka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memcpy(void *dest, const void *src, size_t n)
{
	unsigned char	*dest_ptr;
	unsigned char	*src_ptr;
	int				counter;

	if (dest == NULL && src == NULL)
		return (NULL);
	dest_ptr = (unsigned char*)dest;
	src_ptr = (unsigned char*)src;
	counter = (int)n;
	while (counter--)
		*dest_ptr++ = *src_ptr++;
	return (dest);
}
