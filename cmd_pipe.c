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

t_list	*cmd_lstadd_back_pid(t_list **lst, int pid)
{
	int	*pidptr;

	pidptr = malloc(sizeof(int));
	if (!pidptr)
		return (NULL);
	*pidptr = pid;
	return (ft_lstadd_back_new(lst, (void *)pidptr));
}
