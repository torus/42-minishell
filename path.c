#include "path.h"
#include "env.h"
#include "minishell.h"
#include <string.h>

bool	is_directory(char *path)
{
	struct stat	buf;

	if (stat(path, &buf) != 0)
		return (false);
	if (S_ISDIR(buf.st_mode))
		return (true);
	return (false);
}

/*
 * concatenate dirpath, "/" and filename.
 */
char	*path_join(char *dirpath, char *filename)
{
	char	*tmp;
	char	*result;

	if (ft_strlen(dirpath) == 0 || dirpath[ft_strlen(dirpath) - 1] != '/')
		tmp = ft_strjoin(dirpath, "/");
	else
		tmp = ft_strdup(dirpath);
	if (!tmp)
		return (NULL);
	result = ft_strjoin(tmp, filename);
	free(tmp);
	return (result);
}

/* 親ディレクトリのパスを返す
 *
 * path: 絶対パス.
 * Return: 親ディレクトリの絶対パス.
 */
char	*get_parent_dir(char *path)
{
	char	*result;

	result = ft_substr(path, 0, ft_strrchr(path, '/') - path);
	if (result && ft_strlen(result) == 0)
	{
		free(result);
		result = ft_strdup("/");
	}
	return (result);
}

/* ユーザー が path のファイルのアクセス許可
 *   を持っているか
 */
bool	is_executable(char *path)
{
	struct stat	buf;

	if (stat(path, &buf) == -1)
		return (false);
	if (!(buf.st_mode & S_IXUSR))
		return (false);
	if (!(buf.st_mode & S_IRUSR))
		return (false);
	return (true);
}
