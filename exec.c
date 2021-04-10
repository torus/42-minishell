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
	struct stat		buf;
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
			if (stat(fullpath, &buf) == 0 && S_ISREG(buf.st_mode))
				return (fullpath);
			free(fullpath);
		}
		dirp = readdir(dir);
	}
	return (NULL);
}

char	*find_executable_file_from_path_env(char *filename)
{
	char	*path_env_val;
	char	*executable_path;
	size_t	i;
	size_t	path_len;
	char	*dirpath;

	path_env_val = get_env_val("PATH");
	if (!path_env_val)
		return (NULL);
	i = 0;
	path_len = 0;
	executable_path = NULL;
	while (!executable_path)
	{
		if (path_env_val[i + path_len] == ':' || path_env_val[i + path_len] == '\0')
		{
			if (path_len == 0)
				dirpath = getcwd(NULL, 0);
			else
				dirpath = ft_substr(path_env_val, i, path_len);
			if (!dirpath)
				return (NULL);
			executable_path = find_executable_file_in_dir(filename, dirpath);
			free(dirpath);
			if (path_env_val[i + path_len] == '\0')
				break ;
			i += path_len + 1;
			path_len = 0;
		}
		else
			path_len++;
	}
	free(path_env_val);
	return (executable_path);
}

/*
 * This function works just like execvp.
 *
 * filename: filename or filepath.
 * argv: arguments that is passed to execution program
 *
 * return: If exec command successful, no value will be returned.
 *         This function returning value means exec or other function is failed.
 */
int	ft_execvp(char *filename, char **argv)
{
	char		*executable_path;
	extern char	**environ;

	if (ft_strchr(filename, '/'))
		executable_path = filename;
	else
		executable_path = find_executable_file_from_path_env(filename);
	execve(executable_path, argv, environ);
	return (ERROR);
}
