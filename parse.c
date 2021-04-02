/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thisai <thisai@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/30 19:43:59 by thisai            #+#    #+#             */
/*   Updated: 2021/03/30 20:36:15 by thisai           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <assert.h>
#include <unistd.h>

#define PARSE_BUFFER_SIZE 1024

typedef struct s_parse_buffer
{
	char	buffer[PARSE_BUFFER_SIZE];
	int		size;
	int		cur_pos;
}	t_parse_buffer;

int	parse_getc(t_parse_buffer *buf)
{
	if (buf->cur_pos == buf->size)
	{
		buf->size = read(STDIN_FILENO, buf->buffer, PARSE_BUFFER_SIZE);
		if (buf->size == 0)
			return (EOF);
		buf->cur_pos = 0;
	}
	return (buf->buffer[buf->cur_pos++]);
}

void	parse_ungetc(t_parse_buffer *buf)
{
	assert(buf->cur_pos > 0);
	buf->cur_pos--;
}

typedef enum
{
	TOKTYPE_STRING,
	TOKTYPE_VARIABLE,
	TOKTYPE_PIPE,
	TOKTYPE_INPUT_REDIRECTION,
	TOKTYPE_OUTPUT_REDIRECTION,
	TOKTYPE_OUTPUT_APPENDING,
	TOKTYPE_SEMICOLON,
}	t_parse_token_type;

typedef struct	s_parse_token
{
	char	text[PARSE_BUFFER_SIZE + 1];
	t_parse_token_type	type;
}	t_parse_token;

int	parse_read_word(t_parse_buffer *buf, t_parse_token *result)
{
    return (1);
}

int parse_get_token(t_parse_buffer *buf, t_parse_token *result)
{
	int				ch;

    while (1)
    {
        ch = parse_getc(buf);
        if (ch != ' ' || ch != '\n' || ch != EOF)
            break ;
    }
	if (ch == '$')
	{
        return (parse_read_word(buf, result));
	}
	else if (ch == ';')
	{
	}
	else if (ch == '<')
	{
	}
	else if (ch == '>')
	{
	}
	else if (ch == '"')
	{
	}
	else if (ch == '\'')
	{
	}
}
