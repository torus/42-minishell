#ifndef PARSE_H
# define PARSE_H

# include "token.h"

# define PARSE_BUFFER_SIZE 1024

typedef struct s_parse_buffer
{
	char	buffer[PARSE_BUFFER_SIZE];
	int		size;
	int		cur_pos;
}	t_parse_buffer;

typedef enum e_parse_result
{
	PARSE_OK = 0xc001,
	PARSE_KO,
}	t_parse_result;

typedef enum e_parse_ast_type
{
	ASTNODE_STRING = 0xc201,
	ASTNODE_REDIRECTION,
	ASTNODE_ARGUMENTS,
	ASTNODE_COMMAND,
	ASTNODE_PIPED_COMMANDS,
}	t_parse_ast_type;

typedef struct s_parse_ast_node	t_parse_ast_node;

typedef struct s_parse_node_string
{
	const char				*text;
	t_token_type			type;
	t_parse_ast_node		*next;
}	t_parse_node_string;

typedef struct s_parse_ast_node	t_parse_ast_node;

typedef struct s_parse_node_pipcmds
{
	t_parse_ast_node	*command_node;
	t_parse_ast_node	*next;
}	t_parse_node_pipcmds;

typedef struct s_parse_node_command
{
	t_parse_ast_node	*arguments_node;
}	t_parse_node_command;

typedef struct s_parse_node_arguments
{
	t_parse_ast_node	*string_node;
	t_parse_ast_node	*redirection_node;
	t_parse_ast_node	*rest_node;
}	t_parse_node_arguments;

typedef struct s_parse_node_redirection
{
	t_parse_ast_node	*string_node;
	t_token_type		type;
}	t_parse_node_redirection;

typedef struct s_parse_ast_node
{
	t_parse_ast_type	type;
	union u_parse_ast_node_content
	{
		t_parse_node_redirection	*redirection;
		t_parse_node_string			*string;
		t_parse_node_arguments		*arguments;
		t_parse_node_command		*command;
		t_parse_node_pipcmds		*piped_commands;
	}					content;
}	t_parse_ast_node;

t_parse_result	parse_redirection(
					t_parse_buffer *buf, t_parse_ast_node **node, t_token *tok);
t_parse_result	parse_string(
					t_parse_buffer *buf, t_parse_ast_node **node, t_token *tok);
t_parse_result	parse_arguments(
					t_parse_buffer *buf, t_parse_ast_node **node, t_token *tok);
t_parse_result	parse_command(
					t_parse_buffer *buf, t_parse_ast_node **node, t_token *tok);
t_parse_result	parse_piped_commands(
					t_parse_buffer *buf, t_parse_ast_node **node, t_token *tok);

void			parse_fatal_error(void);

#endif
