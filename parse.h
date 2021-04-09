#ifndef PARSE_H
# define PARSE_H

# define PARSE_BUFFER_SIZE 1024

typedef struct s_parse_buffer
{
	char	buffer[PARSE_BUFFER_SIZE];
	int		size;
	int		cur_pos;
}	t_parse_buffer;

enum e_parse_values
{
	PARSE_OK = 0xc001,
	PARSE_KO,
	TOKTYPE_NON_EXPANDABLE,
	TOKTYPE_EXPANDABLE,
	TOKTYPE_EXPANDABLE_QUOTED,
	TOKTYPE_PIPE,
	TOKTYPE_INPUT_REDIRECTION,
	TOKTYPE_OUTPUT_REDIRECTION,
	TOKTYPE_OUTPUT_APPENDING,
	TOKTYPE_SEMICOLON,
	TOKTYPE_NEWLINE,
	TOKTYPE_SPACE,
	ASTNODE_STRING,
	ASTNODE_REDIRECTION,
};

typedef struct s_parse_token
{
	char	text[PARSE_BUFFER_SIZE];
	int		length;
	int		type;
}	t_parse_token;

typedef struct s_parse_node_string
{
	const char					*text;
	int							type;
	struct s_parse_node_string	*next;
}	t_parse_node_string;

typedef struct s_parse_ast_node	t_parse_ast_node;

typedef struct s_parse_node_redirection
{
	t_parse_ast_node	*string_node;
	int					type;
}	t_parse_node_redirection;

typedef struct s_parse_ast_node
{
	int	type;
	union u_parse_ast_node_content
	{
		t_parse_node_redirection	*redirection;
		t_parse_node_string			*string;
	}	content;
}	t_parse_ast_node;

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

int		parse_redirection(
	t_parse_buffer *buf, t_parse_ast_node **node, t_parse_token *tok);
int		parse_string(
	t_parse_buffer *buf, t_parse_ast_node **node, t_parse_token *tok);

void	parse_fatal_error(void);

#endif
