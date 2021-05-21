#ifndef EXECUTION_H
# define EXECUTION_H

# include <unistd.h>
# include "libft/libft.h"

typedef struct s_cmd_redirection
{
	const char	*filepath;
	bool		is_append;
	int			fd;
}	t_cmd_redirection;

/* コマンド終了後に一気に解放するためにリストで開けたfdを記録する.
 *
 * プロセスが異なる外部コマンドでは気にしなくてもよいのだが,
 * minishellと同じプロセスで動作するビルトインコマンドでは
 * これが必要
 */
typedef struct s_fd_list {
	int					fd;
	struct s_fd_list	*next;
} t_fd_list;
void		fd_list_close(t_fd_list **lst);
t_fd_list	*fd_list_add_fd(t_fd_list **lst, int fd);

typedef struct s_command_invocation
{
	t_list						*output_redirections;
	struct s_command_invocation	*piped_command;
	t_list						*input_redirections;
	const char					**exec_and_args;
	pid_t						pid;
}	t_command_invocation;

char					*find_executable_file_in_dir(
							char *filename, char *dirpath);
char					*find_executable_file_from_path_env(char *filename);
char					*find_executable_file_in_cwd(char *filename);
int						cmd_execvp(char *filename, char **argv);
int						cmd_exec_commands(t_command_invocation *command);
int						cmd_set_input_file(t_command_invocation *command);
int						cmd_set_output_file(t_command_invocation *command, t_fd_list **fd_lst);
void					cmd_exec_command(t_command_invocation *command,
							int pipe_prev_fd[2], int pipe_fd[2]);
void					cmd_close_pipe(int pipe_fd[2]);
void					cmd_copy_pipe(int pipe_new_fd[2], int pipe_fd[2]);
void					cmd_init_pipe_fd(int pipe_fd[2], int pipe0, int pipe1);
t_list					*cmd_lstadd_back_pid(t_list **lst, int pid);
int						cmd_wait_pid_lst(t_list *lst);
t_command_invocation	*cmd_init_cmdinvo(const char **exec_and_args);
t_command_invocation	*cmd_cmdinvo_add_pipcmd(t_command_invocation **cmds,
							t_command_invocation *newcmd);
int						cmd_add_inredirect(t_command_invocation *command,
							const char *filepath, int fd);
int						cmd_add_outredirect(t_command_invocation *command,
							const char *filepath, int fd, bool is_append);
void					cmd_del_redirection(void *redirection);
void					cmd_free_cmdinvo(t_command_invocation *cmds);
char					*expand_env_var(char *str);

#endif
