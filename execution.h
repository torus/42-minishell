#ifndef EXECUTION_H
# define EXECUTION_H

# include <unistd.h>
# include "libft/libft.h"
# include "parse.h"

typedef struct s_cmd_redirection
{
	const char					*filepath;
	bool						is_append;
	bool						is_heredoc;
	int							fd;
	struct s_cmd_redirection	*next;
}	t_cmd_redirection;

t_cmd_redirection		*cmd_copy_redirection(t_cmd_redirection *original);
t_cmd_redirection		*cmd_redirection_add_back(t_cmd_redirection **reds,
							t_cmd_redirection *new_red);
void					cmd_free_redirection(t_cmd_redirection *redirection);
void					cmd_free_redirections(t_cmd_redirection *redirections);

/* This struct is to remember opened filed descriptors
 *   to close them after finish builtin command.
 */
typedef struct s_fd_list {
	int					fd;
	struct s_fd_list	*next;
}	t_fd_list;

/* Keep input redirections or heredocs for each fd.
 *
 * `heredoc_pipe` would be set to `pipe(heredoc_pipe)`
 *   if last element of `reds` is heredoc.
 *
 * You should get this struct through reds2in_fd_reds_list().
 */
typedef struct s_fd_reds_list {
	int							fd;
	t_cmd_redirection			*reds;
	int							heredoc_pipe[2];
	struct s_fd_reds_list		*next;
}	t_fd_reds_list;

t_fd_reds_list			*reds2fd_reds_list(t_cmd_redirection *reds);
void					free_fd_reds_list(t_fd_reds_list *fd_red_list);

typedef struct s_command_invocation
{
	t_cmd_redirection			*output_redirections;
	struct s_command_invocation	*piped_command;
	t_cmd_redirection			*input_redirections;
	const char					**exec_and_args;
	pid_t						pid;
}	t_command_invocation;

char					*find_executable_file_in_dir(
							char *filename, char *dirpath);
char					*find_executable_file_from_path_env(char *filename);
char					*find_executable_file_in_cwd(char *filename);
int						cmd_execvp(char *filename, char **argv);
int						cmd_exec_commands(t_command_invocation *command);
char					*expand_redirect_filepath(char *red_target);
int						open_file_for_redirect(t_cmd_redirection *red,
							int open_flags, mode_t open_mode);
int						put_redir_errmsg_and_ret(int ret_value,
							int fd, char *msg);
int						cmd_set_input_file(t_fd_reds_list *fd_red_list);
int						cmd_set_output_file(t_command_invocation *command);
void					cmd_exec_command(t_command_invocation *command,
							int pipe_prev_fd[2], int pipe_fd[2],
							t_fd_reds_list *in_fd_red_list);
int						cmd_exec_builtin(t_command_invocation *command);
void					cmd_close_pipe(int pipe_fd[2]);
void					cmd_copy_pipe(int pipe_new_fd[2], int pipe_fd[2]);
void					cmd_init_pipe_fd(int pipe_fd[2], int pipe0, int pipe1);
int						cmd_set_heredoc_pipe_fd(
							t_fd_reds_list *in_fd_red_list);
bool					cmd_is_heredoc_expandable(
							t_parse_node_redirection *redirection_node);
char					*expand_heredoc_document(char *str);
t_list					*cmd_lstadd_back_pid(t_list **lst, int pid);
int						cmd_wait_pid_lst(t_list *lst);
t_command_invocation	*cmd_init_cmdinvo(const char **exec_and_args);
t_command_invocation	*cmd_cmdinvo_add_pipcmd(t_command_invocation **cmds,
							t_command_invocation *newcmd);
int						cmd_add_inredirect(t_command_invocation *command,
							const char *filepath, int fd);
int						cmd_add_heredoc(t_command_invocation *command,
							const char *limit_str, int fd, bool is_expandable);
int						cmd_check_readline_has_finished(void);
void					cmd_set_heredoc_sighandlers(void);
int						cmd_add_outredirect(t_command_invocation *command,
							const char *filepath, int fd, bool is_append);
void					cmd_free_cmdinvo(t_command_invocation *cmds);
void					fd_list_close(t_fd_list **lst);
t_fd_list				*fd_list_add_fd(t_fd_list **lst, int fd);

#endif
