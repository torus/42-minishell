/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jtanaka <jtanaka@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/03 03:20:24 by jtanaka           #+#    #+#             */
/*   Updated: 2021/04/03 03:20:25 by jtanaka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	put_err_msg(char *str)
{
	ft_putstr_fd("Error: ", STDERR_FILENO);
	ft_putendl_fd(str, STDERR_FILENO);
}

int	put_err_msg_and_ret(char *str)
{
	put_err_msg(str);
	return (ERROR);
}

void	put_err_msg_and_exit(char *str)
{
	put_err_msg(str);
	exit(EXIT_FAILURE);
}
