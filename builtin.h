#ifndef BUILTIN
# define BUILTIN

# include <stdlib.h>
# include <stdbool.h>
# include <unistd.h>
# include <limits.h>
# include <stdio.h>
# include "libft/libft.h"

// ビルトインコマンドは argv を受け取って, ステータスを返す
typedef int t_builtin_cmd(char**);

bool			is_builtin_command(char *cmd_name);
t_builtin_cmd	*get_builtin_func(char *cmd_name);
int				builtin_echo(char **argv);
int				builtin_cd(char **argv);
int				builtin_pwd(char **argv);

#endif
