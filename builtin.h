#ifndef BUILTIN
# define BUILTIN

# include <stdlib.h>
# include <stdbool.h>
# include <unistd.h>
# include <stdio.h>
# include "libft/libft.h"

// ビルトインコマンドは argv を受け取って, ステータスを返す
typedef int t_builtin_cmd(char**);

bool	is_builtin_command(char *cmd_name);
int		echo(char **argv);

#endif
