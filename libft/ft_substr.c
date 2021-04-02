/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jtanaka <jtanaka@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/11 02:36:34 by jtanaka           #+#    #+#             */
/*   Updated: 2021/04/02 18:54:57 by jtanaka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char			*substr;

	substr = malloc(len + 1);
	if (!substr)
		return (NULL);
	*substr = '\0';
	if (start >= ft_strlen(s))
		return (substr);
	ft_strlcpy(substr, s + start, len + 1);
	return (substr);
}
