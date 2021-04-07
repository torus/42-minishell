#include "env.h"
#include "execution.h"

char	*find_executable_file(char *filename)
{

}

char	*find_executable_file_from_path_env(char *filename)
{
	char *path_env_val;
	char **paths;
	size_t idx;

	// find "hoge:hoge" from "PATH=hoge:hoge"
	path_env_val = get_env_val("PATH");
	if (!path_env_val)
		return (NULL);
	// split path_env_val with ':'
	paths = ft_split(path_env_val, ':');
	if (!paths)
		return (NULL);
	idx = 0;
	while (paths[idx])
	{
		if (find_executable_file_in_dir(filename, paths[idx]))
			break;
		idx++;
	}
	return (find_executable_file_in_dir(filename, paths[idx]));
}

int	ft_execvp(char *filename, char **argv)
{
	execve(filename, argv, __environ);
	return (ERROR);
}

