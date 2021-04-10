#ifndef TOKEN_H
# define TOKEN_H

# define TOKEN_BUFFER_SIZE 1024

typedef struct s_parse_buffer	t_parse_buffer;

typedef enum e_token_type
{
	TOKTYPE_NON_EXPANDABLE = 0xc101,
	TOKTYPE_EXPANDABLE,
	TOKTYPE_EXPANDABLE_QUOTED,
	TOKTYPE_PIPE,
	TOKTYPE_INPUT_REDIRECTION,
	TOKTYPE_OUTPUT_REDIRECTION,
	TOKTYPE_OUTPUT_APPENDING,
	TOKTYPE_SEMICOLON,
	TOKTYPE_NEWLINE,
	TOKTYPE_SPACE,
}	t_token_type;

typedef struct s_token
{
	char	text[TOKEN_BUFFER_SIZE];
	int		length;
	int		type;
}	t_token;

int		token_getc(t_parse_buffer *buf);
void	token_ungetc(t_parse_buffer *buf);
int		token_is_special_char(char ch);
int		token_read_word(t_parse_buffer *buf, t_token *result);
int		token_read_double_quoted(t_parse_buffer *buf, t_token *result);
int		token_read_single_quoted(t_parse_buffer *buf, t_token *result);
int		token_get_spaces(t_parse_buffer *buf, t_token *result, int ch);
int		token_get_symbols(t_parse_buffer *buf, t_token *result, int ch);
int		token_get_quoted(t_parse_buffer *buf, t_token *result, int ch);
int		token_get_token(t_parse_buffer *buf, t_token *result);

#endif
