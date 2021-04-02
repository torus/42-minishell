/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jtanaka <jtanaka@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/12 02:42:43 by jtanaka           #+#    #+#             */
/*   Updated: 2021/04/02 20:22:33 by jtanaka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	itoa_recursive(char *result, int n)
{
	int	idx;
	int	sign;

	idx = 0;
	sign = 1;
	if (n / 10)
		idx = itoa_recursive(result, n / 10);
	else if (n < 0)
		result[idx++] = '-';
	if (n < 0)
		sign = -1;
	result[idx++] = '0' + sign * (n % 10);
	result[idx] = '\0';
	return (idx);
}

char	*ft_itoa(int n)
{
	int			arr_size;
	char		*result;

	arr_size = get_digit(n) + 1;
	if (n < 0)
		arr_size++;
	result = (char *)ft_calloc(arr_size, sizeof(char));
	if (!result)
		return (NULL);
	itoa_recursive(result, n);
	return (result);
}
