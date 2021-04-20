#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdlib.h>
# include <unistd.h>
# include <stdio.h>
# include "execution.h"
# include "path.h"
# include "lexer.h"
# include "parse.h"

// AST to command_invocation
int		cmd_ast_pipcmds2cmdinvo(t_parse_node_pipcmds *pipcmds);

#endif
