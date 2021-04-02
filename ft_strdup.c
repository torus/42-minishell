/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jtanaka <jtanaka@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/09 04:07:10 by jtanaka           #+#    #+#             */
/*   Updated: 2021/04/02 20:13:43 by jtanaka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strdup(const char *s)
{
	size_t			s_len;
	char			*new_str;

	s_len = ft_strlen(s);
	new_str = (char *)malloc(s_len + 1);
	if (!new_str)
		return (NULL);
	ft_strlcpy(new_str, s, s_len + 1);
	return (new_str);
}
