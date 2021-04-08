#ifndef EXECUTION_H
# define EXECUTION_H

# include <unistd.h>
# include <stdio.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <signal.h>
# include <dirent.h>
# include "libft/libft.h"

# define MAXLINE 1000

typedef struct s_command_invocation
{
    const char              *output_file_path; /* > file の場合 */
    struct s_command_invocation    *piped_command; /* | command の場合（> と | は同時には指定できない） */
    const char              *input_file_path; /* < file */
    const char              **exec_and_args; /* ft_split で区切ったコマンドのパスと引数のリスト */
}   t_command_invocation;


/* Signal handlers */
static void	sig_int(int signo);  /* Ctrl + C */
static void	sig_quit(int signo); /* Ctrl + \ */

/* Execution */
char	*find_executable_file_in_dir(char *filename, char *dirpath);
char	*find_executable_file_from_path_env(char *filename);
char	*find_executable_file(char *filename);
int		ft_execvp(char *filename, char **argv);

/* path */
char	*path_join(char *dirpath, char *filename);

#endif

