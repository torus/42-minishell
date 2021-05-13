#ifndef LEXER_H
# define LEXER_H

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

typedef enum e_lexer_state
{
    LEXSTAT_NORMAL = 0xd101,
    LEXSTAT_SINGLE_QUOTED,
    LEXSTAT_DOUBLE_QUOTED,
}	t_lexer_state;

typedef struct s_token
{
	char			text[TOKEN_BUFFER_SIZE];
	int				length;
	t_token_type	type;
}	t_token;

int		lex_getc(t_parse_buffer *buf);
void	lex_ungetc(t_parse_buffer *buf);
int		lex_is_special_char(char ch);
int		lex_read_word(t_parse_buffer *buf, t_token *result);
int		lex_read_double_quoted(t_parse_buffer *buf, t_token *result);
int		lex_read_single_quoted(t_parse_buffer *buf, t_token *result);
int		lex_get_spaces(t_parse_buffer *buf, t_token *result, int ch);
int		lex_get_symbols(t_parse_buffer *buf, t_token *result, int ch);
int		lex_get_quoted(t_parse_buffer *buf, t_token *result, int ch);
int		lex_get_token(t_parse_buffer *buf, t_token *result);
int		lex_check_redirection_with_fd(t_parse_buffer *buf, t_token *result);

#endif
