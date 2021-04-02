/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jtanaka <jtanaka@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/25 02:22:38 by jtanaka           #+#    #+#             */
/*   Updated: 2021/04/02 20:01:40 by jtanaka          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

static int	join_line_from_save(char **line, char **save)
{
	char	*tmp;

	if (ft_strchr(*save, '\n'))
	{
		tmp = *line;
		*line = ft_substr(*save, 0, ft_strchr(*save, '\n') - *save);
		free(tmp);
		if (!(*line))
			return (ERROR);
		tmp = *save;
		*save = ft_substr(ft_strchr(*save, '\n') + 1, 0,
				ft_strlen(ft_strchr(*save, '\n') + 1));
		free(tmp);
		if (!(*save))
			return (ERROR);
		return (SUCCESS);
	}
	else
	{
		tmp = *line;
		*line = *save;
		*save = NULL;
		free(tmp);
		return (CONTINUE_READ);
	}
}

static int	split_by_newline(char **line, char **save, char *buf)
{
	char	*old_line;
	char	*tmp;

	tmp = ft_substr(buf, 0, ft_strchr(buf, '\n') - buf);
	if (!tmp)
		return (ERROR);
	old_line = *line;
	*line = ft_strjoin(*line, tmp);
	free(old_line);
	free(tmp);
	if (!(*line))
		return (ERROR);
	*save = ft_substr(ft_strchr(buf, '\n') + 1, 0,
			ft_strlen(ft_strchr(buf, '\n') + 1));
	if (!(*save))
		return (ERROR);
	return (SUCCESS);
}

static int	join_line_and_buf(char **line, char *buf)
{
	char	*tmp;

	tmp = *line;
	*line = ft_strjoin(*line, buf);
	free(tmp);
	if (!(*line))
		return (ERROR);
	return (CONTINUE_READ);
}

static int	read_process(int fd, char **line, char **save)
{
	ssize_t		read_size;
	int			ret;
	char		*buf;

	ret = CONTINUE_READ;
	buf = malloc(BUFFER_SIZE + 1);
	if (!buf)
		return (ERROR);
	while (ret == CONTINUE_READ)
	{
		read_size = read(fd, buf, BUFFER_SIZE);
		if (read_size <= 0)
			break ;
		buf[read_size] = '\0';
		if (ft_strchr(buf, '\n'))
			ret = split_by_newline(line, save, buf);
		else
			ret = join_line_and_buf(line, buf);
	}
	free(buf);
	if (ret == CONTINUE_READ && read_size == 0)
		ret = END_OF_FILE;
	else if (ret == CONTINUE_READ && read_size == -1)
		ret = ERROR;
	return (ret);
}

int	get_next_line(int fd, char **line)
{
	int					ret;
	static t_gnl_list	*save_list_head;
	t_gnl_list			*target_save_list;

	*line = ft_calloc(1, sizeof(char));
	if (fd < 0 || !line || BUFFER_SIZE <= 0 || !(*line))
		return (ERROR);
	target_save_list = save_list_head;
	while (target_save_list && target_save_list->fd != fd)
		target_save_list = target_save_list->next;
	if (!target_save_list)
	{
		target_save_list = create_fd_elem(&save_list_head, fd);
		if (!target_save_list)
			return (ERROR);
	}
	ret = CONTINUE_READ;
	if (target_save_list->save)
		ret = join_line_from_save(line, &target_save_list->save);
	if (ret == CONTINUE_READ)
		ret = read_process(fd, line, &target_save_list->save);
	if (ret == END_OF_FILE || ret == ERROR)
		remove_fd_elem(&save_list_head, target_save_list);
	return (ret);
}
