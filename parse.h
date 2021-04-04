#ifndef PARSE_H
# define PARSE_H

# define PARSE_BUFFER_SIZE 1024

typedef struct s_parse_buffer
{
	char	buffer[PARSE_BUFFER_SIZE];
	int		size;
	int		cur_pos;
}	t_parse_buffer;

typedef enum e_parse_token_type
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

typedef struct s_parse_token
{
	char				text[PARSE_BUFFER_SIZE];
	int					length;
	t_parse_token_type	type;
}	t_parse_token;

int		parse_getc(t_parse_buffer *buf);
void	parse_ungetc(t_parse_buffer *buf);
int		parse_is_special_char(char ch);
int		parse_read_word(t_parse_buffer *buf, t_parse_token *result);
int		parse_read_double_quoted(t_parse_buffer *buf, t_parse_token *result);
int		parse_read_single_quoted(t_parse_buffer *buf, t_parse_token *result);
int		parse_get_spaces(t_parse_buffer *buf, t_parse_token *result, int ch);
int		parse_get_symbols(t_parse_buffer *buf, t_parse_token *result, int ch);
int		parse_get_quoted(t_parse_buffer *buf, t_parse_token *result, int ch);
int		parse_get_token(t_parse_buffer *buf, t_parse_token *result);

#endif
