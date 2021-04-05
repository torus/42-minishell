#include "execution.h"

char	*find_executable_file(char *filename)
{
	return (NULL);
}

int	ft_execvp(char *filename, char **argv)
{
	execve(filename, argv, __environ);
	return (ERROR);
}

