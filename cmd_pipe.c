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

int	cmd_set_heredoc_pipe_fd(t_fd_reds_list *in_fd_red_list)
{
	t_cmd_redirection	*red;

	while (in_fd_red_list)
	{
		red = in_fd_red_list->reds;
		while (red->next)
			red = red->next;
		if (red->is_heredoc && pipe(in_fd_red_list->heredoc_pipe) == -1)
			return (ERROR);
		in_fd_red_list = in_fd_red_list->next;
	}
	return (0);
}
