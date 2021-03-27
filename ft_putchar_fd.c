/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putchar_fd.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jtanaka <jtanaka@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/12 03:29:18 by jtanaka           #+#    #+#             */
/*   Updated: 2020/10/20 16:20:24 by jtanaka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_putchar_fd(char c, int fd)
{
	unsigned char head;
	unsigned char tail;

	if ((unsigned char)c <= 0x7f)
		write(fd, &c, 1);
	else
	{
		head = (((unsigned char)c & 0b11000000) >> 6) | 0b11000000;
		tail = ((unsigned char)c & 0b00111111) | 0b10000000;
		write(fd, &head, 1);
		write(fd, &tail, 1);
	}
}
