#ifndef BUILTIN_H
# define BUILTIN_H

# include <stdlib.h>
# include <stdbool.h>
# include <unistd.h>
# include <limits.h>
# include <stdio.h>
# include <errno.h>
# include "libft/libft.h"
# include "env.h"

// ビルトインコマンドは argv を受け取って, ステータスを返す
typedef int	t_builtin_cmd(char**);
bool			is_builtin_command(char *cmd_name);
t_builtin_cmd	*get_builtin_func(char *cmd_name);
int				builtin_echo(char **argv);
int				builtin_cd(char **argv);
bool			change_directory(char *dest);
bool			will_search_cdpath(char **argv, char *dest);
int				cd_cdpath_env(char *dest_path);
int				builtin_pwd(char **argv);
int				builtin_env(char **argv);
int				builtin_export(char **argv);
int				builtin_unset(char **argv);
int				builtin_exit(char **argv);

#endif
