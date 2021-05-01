#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdlib.h>
# include <unistd.h>
# include <stdio.h>
# include "execution.h"
# include "env.h"
# include "path.h"
# include "lexer.h"
# include "parse.h"
# include "builtin.h"

# define PROMPT "minish > "

// AST to command_invocation
t_command_invocation	*cmd_ast_pipcmds2cmdinvo(t_parse_node_pipcmds *pipcmds);
t_command_invocation	*cmd_ast_cmd2cmdinvo(t_parse_node_command *cmd_node);
char					*string_node2string(t_parse_node_string *string_node);
char					**expand_string_node(t_parse_node_string *string_node);
char					**split_expanded_str(char *str);
char					**expand_string_node(t_parse_node_string *string_node);
void					set_shell_sighandlers(void);
void					set_sighandlers(__sighandler_t sighandler);

#endif
