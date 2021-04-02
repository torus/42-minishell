/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jtanaka <jtanaka@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/11 04:52:46 by jtanaka           #+#    #+#             */
/*   Updated: 2021/04/02 18:56:48 by jtanaka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <stdio.h>

int	get_split_size(char *str, char c)
{
	int		split_size;

	split_size = 0;
	while (*str)
	{
		while (*str && *str == c)
			str++;
		if (!*str)
			break ;
		while (*str && *str != c)
			str++;
		split_size++;
	}
	return (split_size);
}

static void	free_n(char **arr, size_t n)
{
	size_t	i;

	i = 0;
	while (i < n)
		free(arr[i++]);
	free(arr);
}

static char	**split(char **result, char *str, char c)
{
	size_t	i;
	size_t	len;

	i = 0;
	len = 0;
	while (*str)
	{
		while (*str && *str == c)
			str++;
		if (!*str)
			break ;
		len = 0;
		while (str[len] && str[len] != c)
			len++;
		result[i] = ft_substr(str, 0, len);
		if (!result[i])
		{
			free_n(result, i);
			return (NULL);
		}
		i++;
		str += len;
	}
	result[i] = NULL;
	return (result);
}

char	**ft_split(char const *s, char c)
{
	int		split_size;
	char	**result;

	split_size = get_split_size((char *)s, c);
	result = (char **)malloc(sizeof(char *) * (split_size + 1));
	if (!result)
		return (NULL);
	return (split(result, (char *)s, c));
}
