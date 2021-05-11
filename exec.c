#include "env.h"
#include "execution.h"
#include "minishell.h"
#include <string.h>
#include <errno.h>

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
				return (free_and_rtn_ptr(dir, NULL));
			if (stat(fullpath, &buf) == 0 && S_ISREG(buf.st_mode))
				return (free_and_rtn_ptr(dir, fullpath));
			free(fullpath);
		}
		dirp = readdir(dir);
	}
	return (free_and_rtn_ptr(dir, NULL));
}

/*
 * Find filename in directories which is listed in dirpaths_str.
 *
 * filename: program name that is searched.
 * dirpaths_str: string that has directory paths which is delimited with colon.
 *
 * return: filepath if executable file is found, otherwise return NULL.
 */
static char	*get_executable_filepath(char *filename, const char *dirpaths_str)
{
	char	*executable_path;
	size_t	path_len;
	char	*dirpath;

	path_len = 0;
	executable_path = NULL;
	while (!executable_path)
	{
		if (dirpaths_str[path_len] == ':' || dirpaths_str[path_len] == '\0')
		{
			if (path_len == 0)
				dirpath = getcwd(NULL, 0);
			else
				dirpath = ft_substr(dirpaths_str, 0, path_len);
			executable_path = find_executable_file_in_dir(filename, dirpath);
			free(dirpath);
			if (dirpaths_str[path_len] == '\0')
				break ;
			dirpaths_str += path_len + 1;
			path_len = 0;
		}
		else
			path_len++;
	}
	return (executable_path);
}

/*
 * Find executable file in directories which is listed in $PATH.
 *
 * filename: program name that is searched.
 *
 * return: filepath if executable file is found, otherwise return NULL.
 */
char	*find_executable_file_from_path_env(char *filename)
{
	char	*path_env_val;
	char	*executable_path;

	path_env_val = get_env_val("PATH");
	if (!path_env_val)
		return (NULL);
	executable_path = get_executable_filepath(filename, path_env_val);
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
int	cmd_execvp(char *filename, char **argv)
{
	char		*executable_path;
	extern char	**environ;

	if (ft_strchr(filename, '/'))
		executable_path = filename;
	else
		executable_path = find_executable_file_from_path_env(filename);
	execve(executable_path, argv, environ);
	put_minish_err_msg(executable_path, strerror(errno));
	free(executable_path);
	return (ERROR);
}
