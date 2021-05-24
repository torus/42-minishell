#ifndef MINISHELL_H
# define MINISHELL_H

# include <signal.h>
# include "execution.h"
# include "parse.h"
# include "env.h"

# define PROMPT "minish > "

// グローバル構造体
typedef struct	s_shell {
	char		*cwd;
	t_var	*vars;
}				t_shell;
extern t_shell	g_shell;

// AST から exec_and_args に変換する時に使う構造体
typedef struct s_cmd_str_node {
	char					*text;
	t_token_type			type;
}				t_cmd_str_node;

// MacOSでは signal.h に __sighandler_t がない
typedef void	(*t_sighandler)(int);

// AST to command_invocation
t_command_invocation	*cmd_ast_pipcmds2cmdinvo(t_parse_node_pipcmds *pipcmds);
t_command_invocation	*cmd_ast_cmd2cmdinvo(t_parse_node_command *cmd_node);
char					*string_node2string(t_parse_node_string *string_node);
char					**expand_string_node(t_parse_node_string *string_node);
char					**split_expanded_str(char *str);
char					**expand_string_node(t_parse_node_string *string_node);
void					set_shell_sighandlers(void);
void					set_sighandlers(t_sighandler sighandler);
void					put_minish_err_msg(const char *cmd_name,
							const char *msg);
int						put_minish_err_msg_and_ret(int ret_val,
							const char *cmd_name, const char *msg);
void					put_minish_err_msg_and_exit(int status,
							const char *cmd_name, const char *msg);

// シェルの初期化
int						initialize_shell(void);

#endif
