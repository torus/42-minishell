#ifndef EXECUTION_H
# define EXECUTION_H

# include <unistd.h>
# include <stdio.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <signal.h>
# include <dirent.h>
# include "libft/libft.h"
# include "path.h"

typedef struct s_command_invocation
{
	const char					*output_file_path;
	struct s_command_invocation	*piped_command;
	const char					*input_file_path;
	const char					**exec_and_args;
}	t_command_invocation;

char	*find_executable_file_in_dir(char *filename, char *dirpath);
char	*find_executable_file_from_path_env(char *filename);
char	*find_executable_file_in_cwd(char *filename);
int		cmd_execvp(char *filename, char **argv);
int		cmd_command_execution(t_command_invocation *command);
int		cmd_set_input_file(t_command_invocation *command);
int		cmd_set_output_file(t_command_invocation *command);
int		cmd_pipe_process(t_command_invocation *command, int pipe_fd[2]);
int		cmd_spawn_child(t_command_invocation *command);

#endif
