#include "execution.h"

void	cmd_close_pipe(int pipe_fd[2])
{
	close(pipe_fd[0]);
	close(pipe_fd[1]);
}

void	cmd_copy_pipe(int pipe_new_fd[2], int pipe_fd[2])
{
	pipe_new_fd[0] = pipe_fd[0];
	pipe_new_fd[1] = pipe_fd[1];
}

void	cmd_init_pipe_fd(int pipe_fd[2], int pipe0, int pipe1)
{
	pipe_fd[0] = pipe0;
	pipe_fd[1] = pipe1;
}

int	cmd_set_heredoc_pipe_fd(t_command_invocation *command,
	int pipe_heredoc_fd[2])
{
	t_list				*current;
	t_cmd_redirection	*red;

	if (!command->input_redirections)
	{
		pipe_heredoc_fd[0] = -1;
		pipe_heredoc_fd[1] = -1;
		return (0);
	}
	current = command->input_redirections;
	while (current->next)
		current = current->next;
	red = (t_cmd_redirection *)current->content;
	if (!red->is_heredoc)
	{
		pipe_heredoc_fd[0] = -1;
		pipe_heredoc_fd[1] = -1;
		return (0);
	}
	return (pipe(pipe_heredoc_fd));
}
