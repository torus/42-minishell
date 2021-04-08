#include "env.h"
#include "execution.h"

/*
 * find executable file in dirpath.
 *
 *   filename: filename
 *   dirpath: the directory to search
 *
 *   return:  fullpath of found executable file,
 *            otherwise return NULL.
 */
char	*find_executable_file_in_dir(char *filename, char *dirpath)
{
	DIR				*dir;
	struct dirent	*dirp;
	struct stat		*buf;
	char			*fullpath;

	dir = opendir(dirpath);
	if (!dir)
		return (NULL);
	dirp = readdir(dir);
	while (dirp)
	{
		if (ft_strcmp(dirp->d_name, filename) == 0)
		{
			fullpath = path_join(dirpath, dirp->d_name);
			if (!fullpath)
				return (NULL);
			if (stat(fullpath, buf) && S_ISREG(buf->st_mode))
				return (fullpath);
			free(fullpath);
		}
		dirp = readdir(dir);
	}
	return (NULL);
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

