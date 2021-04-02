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
	TOKTYPE_NON_EXPANDABLE,
	TOKTYPE_EXPANDABLE,
	TOKTYPE_PIPE,
	TOKTYPE_INPUT_REDIRECTION,
	TOKTYPE_OUTPUT_REDIRECTION,
	TOKTYPE_OUTPUT_APPENDING,
	TOKTYPE_SEMICOLON,
	TOKTYPE_NEWLINE,
	TOKTYPE_SPACE,
}	t_parse_token_type;

typedef struct	s_parse_token
{
	char				text[PARSE_BUFFER_SIZE];
    int					length;
	t_parse_token_type	type;
}	t_parse_token;

int	parse_is_special_char(char ch)
{
    if (ch == ' ' || ch == '\t' || ch == '\n' || ch == ';'
        || ch == '|' || ch == '>' || ch == '<' || ch == '"' || ch == '\'')
        return (1);
    return (0);
}

int	parse_read_word(t_parse_buffer *buf, t_parse_token *result)
{
    int	pos;
    int	ch;

    pos = 0;
    while (1)
    {
        ch = parse_getc(buf);
        if (ch == EOF)
            break ;
        if (parse_is_special_char(ch))
        {
            parse_ungetc(buf);
            break ;
        }
        result->text[pos++] = ch;
    }
    result->length = pos;
    return (1);
}

int	parse_read_double_quoted(t_parse_buffer *buf, t_parse_token *result)
{
    int	pos;
    int	ch;

    pos = 0;
    while (1)
    {
        ch = parse_getc(buf);
        if (ch == '"' || ch == '\n' || ch == EOF)
            break ;
        result->text[pos++] = ch;
    }
    result->length = pos;
    return (1);
}

int	parse_read_single_quoted(t_parse_buffer *buf, t_parse_token *result)
{
    int	pos;
    int	ch;

    pos = 0;
    while (1)
    {
        ch = parse_getc(buf);
        if (ch == '\'' || ch == '\n' || ch == EOF)
            break ;
        result->text[pos++] = ch;
    }
    result->length = pos;
    return (1);
}

int parse_get_token(t_parse_buffer *buf, t_parse_token *result)
{
	int				ch;

    ch = parse_getc(buf);
    if (ch == ' ' || ch == '\t')
    {
        while (1)
        {
            ch = parse_getc(buf);
            if (ch != ' ' && ch != '\t')
            {
                if (ch != EOF)
                    parse_ungetc(buf);
                result->type = TOKTYPE_SPACE;
                break ;
            }
        }
    }
    else if (ch == '\n')
	{
        result->type = TOKTYPE_NEWLINE;
        return (1);
	}
	else if (ch == ';')
	{
        result->type = TOKTYPE_SEMICOLON;
        return (1);
	}
	else if (ch == '|')
	{
        result->type = TOKTYPE_PIPE;
        return (1);
	}
	else if (ch == '<')
	{
        result->type = TOKTYPE_INPUT_REDIRECTION;
        return (1);
	}
	else if (ch == '>')
	{
        ch = parse_getc(buf);
        if (ch == '>')
            result->type = TOKTYPE_OUTPUT_APPENDING;
        else
        {
            result->type = TOKTYPE_OUTPUT_REDIRECTION;
            if (ch != EOF)
                parse_ungetc(buf);
        }
        return (1);
	}
	else if (ch == '"')
	{
        result->type = TOKTYPE_EXPANDABLE;
        return (parse_read_double_quoted(buf, result));
	}
	else if (ch == '\'')
	{
        result->type = TOKTYPE_NON_EXPANDABLE;
        return (parse_read_single_quoted(buf, result));
	}
    else if (ch != EOF)
    {
        result->type = TOKTYPE_EXPANDABLE;
        parse_ungetc(buf);
        return (parse_read_word(buf, result));
    }
}
