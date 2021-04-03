/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jtanaka <jtanaka@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/09 02:58:56 by jtanaka           #+#    #+#             */
/*   Updated: 2021/04/02 17:12:12 by jtanaka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	num_len(const char *str)
{
	int	i;

	i = 0;
	while (ft_isdigit(*str++))
		i++;
	return (i);
}

static bool	is_overflow(const char *nptr, int sign)
{
	if (num_len(nptr) > 19)
		return (true);
	if (num_len(nptr) == 19)
	{
		if (sign == -1 && ft_strncmp(nptr, "9223372036854775808", 19) > 0)
			return (true);
		if (sign == 1 && ft_strncmp(nptr, "9223372036854775807", 19) > 0)
			return (true);
	}
	return (false);
}

int	ft_atoi(const char *nptr)
{
	unsigned int	num;
	int				np;

	np = 1;
	num = 0;
	while (*nptr == ' ' || *nptr == '\t' || *nptr == '\f'
		|| *nptr == '\r' || *nptr == '\n' || *nptr == '\v')
		nptr++;
	if (*nptr == '+' || *nptr == '-')
		if (*nptr++ == '-')
			np = -1;
	if (is_overflow(nptr, np))
	{
		if (np == 1)
			return (-1);
		else
			return (0);
	}
	while (ft_isdigit(*nptr))
		num = num * 10 + (*nptr++ - '0');
	return ((int)(np * num));
}
