#ifndef BUILTIN_H
# define BUILTIN_H

# include <stdbool.h>

// Builtin commands require argv (array of string) as arguments.
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
int				print_envs_with_declaration(void);
int				builtin_export(char **argv);
int				builtin_unset(char **argv);
int				builtin_exit(char **argv);

#endif
