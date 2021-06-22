#ifndef MINISHELL_H
# define MINISHELL_H

# include <signal.h>
# include "execution.h"
# include "parse.h"
# include "env.h"

# define MINISHELL_PROMPT "minish > "
# define MINISHELL_PROMPT_LEN 9

typedef struct s_shell {
	char				*cwd;
	t_var				*vars;
	int					status;
	int					signal_child_received;
	int					heredoc_interruption;
}				t_shell;
extern t_shell	g_shell;

/* This struct is used in process
 *   that convert AST string node to exec_and_args (array of string).
 */
typedef struct s_cmd_str_node {
	char					*text;
	t_token_type			type;
}				t_cmd_str_node;

// FreeBSD doesn't have type __sighandler_t.
typedef void	(*t_sighandler)(int);

t_command_invocation	*cmd_ast_pipcmds2cmdinvo(t_parse_node_pipcmds *pipcmds);
t_command_invocation	*cmd_ast_cmd2cmdinvo(t_parse_node_command *cmd_node);
char					*string_node2string(t_parse_node_string *string_node,
							bool add_quotes);
char					**expand_string_node(t_parse_node_string *string_node,
							bool is_export_cmd);
char					**split_expanded_str(char *str);
void					set_shell_sighandlers(void);
void					set_sighandlers_during_execution(void);
void					set_sighandlers(t_sighandler sighandler);
void					put_minish_err_msg(const char *cmd_name,
							const char *msg);
int						put_minish_err_msg_and_ret(int ret_val,
							const char *cmd_name, const char *msg);
void					put_minish_err_msg_and_exit(int status,
							const char *cmd_name, const char *msg);
void					check_malloc_has_succeeded(char *cmd_name, void *ptr);
int						invoke_sequential_commands(t_parse_ast *seqcmd);
int						interactive_shell(void);

// Shell initialization
void					init_g_shell(void);
int						initialize_shell(void);

#endif
