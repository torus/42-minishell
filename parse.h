#ifndef PARSE_H
# define PARSE_H

# include "lexer.h"

# define PARSE_BUFFER_SIZE 1024

typedef struct s_parse_buffer
{
	char	buffer[PARSE_BUFFER_SIZE];
	int		size;
	int		cur_pos;
}	t_parse_buffer;

typedef enum e_parse_ast_type
{
	ASTNODE_NONE = 0xc201,
	ASTNODE_STRING,
	ASTNODE_REDIRECTION,
	ASTNODE_ARGUMENTS,
	ASTNODE_COMMAND,
	ASTNODE_PIPED_COMMANDS,
	ASTNODE_DELIMITER,
	ASTNODE_SEQ_COMMANDS,
	ASTNODE_COMMAND_LINE,
	ASTNODE_INVALID,
}	t_parse_ast_type;

typedef struct s_parse_ast	t_parse_ast;

typedef struct s_parse_node_string
{
	char			*text;
	t_token_type	type;
	t_parse_ast		*next;
}	t_parse_node_string;

typedef struct s_parse_node_pipcmds
{
	t_parse_ast	*command_node;
	t_parse_ast	*next;
}	t_parse_node_pipcmds;

typedef struct s_parse_node_command
{
	t_parse_ast	*arguments_node;
}	t_parse_node_command;

typedef struct s_parse_node_arguments
{
	t_parse_ast	*string_node;
	t_parse_ast	*redirection_node;
	t_parse_ast	*rest_node;
}	t_parse_node_arguments;

typedef struct s_parse_node_redirection
{
	t_parse_ast		*string_node;
	t_token_type	type;
}	t_parse_node_redirection;

typedef struct s_parse_node_delimiter
{
	t_token_type	type;
}	t_parse_node_delimiter;

typedef struct s_parse_node_seqcmds
{
	t_parse_ast	*pipcmd_node;
	t_parse_ast	*delimiter_node;
	t_parse_ast	*rest_node;
}	t_parse_node_seqcmds;

typedef struct s_parse_node_cmdline
{
	t_parse_ast	*seqcmd_node;
	t_parse_ast	*delimiter_node;
}	t_parse_node_cmdline;

typedef struct s_parse_ast
{
	t_parse_ast_type	type;
	union u_parse_ast_node_content
	{
		void						*void_ptr;
		t_parse_node_redirection	*redirection;
		t_parse_node_string			*string;
		t_parse_node_arguments		*arguments;
		t_parse_node_command		*command;
		t_parse_node_pipcmds		*piped_commands;
		t_parse_node_delimiter		*delimiter;
		t_parse_node_seqcmds		*sequential_commands;
		t_parse_node_cmdline		*command_line;
	}					content;
}	t_parse_ast;

t_parse_ast	*parse_new_ast_node(t_parse_ast_type type, void *content);

t_parse_ast	*parse_redirection(t_parse_buffer *buf, t_token *tok);
t_parse_ast	*parse_string(t_parse_buffer *buf, t_token *tok);
t_parse_ast	*parse_arguments(t_parse_buffer *buf, t_token *tok);
t_parse_ast	*parse_command(t_parse_buffer *buf, t_token *tok);
t_parse_ast	*parse_piped_commands(t_parse_buffer *buf, t_token *tok);
t_parse_ast	*parse_delimiter(t_parse_buffer *buf, t_token *tok);
t_parse_ast	*parse_sequential_commands(t_parse_buffer *buf, t_token *tok);
t_parse_ast	*parse_command_line(t_parse_buffer *buf, t_token *tok);

void		parse_fatal_error(void);
void		parse_skip_spaces(t_parse_buffer *buf, t_token *tok);

typedef struct s_parse_ast_list
{
	t_parse_ast				ast;
	struct s_parse_ast_list	*next;
}	t_parse_ast_list;

void		parse_free_all_ast(void);

#endif
