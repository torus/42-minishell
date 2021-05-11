#include "path.h"
#include "env.h"
#include "minishell.h"
#include <string.h>

/* 親ディレクトリのパスを返す
 */
static char	*get_parent_dir(char *path)
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

static void	update_path(char **path, char *retrieve_path)
{
	char	*tmp;

	if (ft_strncmp(retrieve_path, ".", 2) == 0)
		return ;
	else if (ft_strncmp(retrieve_path, "..", 3) == 0)
	{
		tmp = *path;
		*path = get_parent_dir(*path);
		free(tmp);
	}
	else
	{
		tmp = *path;
		*path = path_join(*path, retrieve_path);
		free(tmp);
	}
}

static bool	is_double_slash(char *path)
{
	return (ft_strlen(path) >= 2
		&& path[0] == '/' && path[1] == '/' && path[2] != '/');
}

static void	add_double_slash(char **path)
{
	char	*tmp;

	tmp = *path;
	*path = ft_strjoin("/", *path);
	free(tmp);
}

/* パスを正規化する.
 * ex:
 *   - "////hoge//fuga//../././//gaga" -> "/hoge/gaga"
 */
char	*canonicalize_path(char *path)
{
	char	**dirs;
	char	*result;
	int		i;

	dirs = ft_split(path, '/');
	i = 0;
	result = ft_strdup("/");
	while (dirs[i])
	{
		update_path(&result, dirs[i]);
		if (!result || !is_directory(result))
		{
			free(result);
			free_ptrarr((void **)dirs);
			return (NULL);
		}
		i++;
	}
	free_ptrarr((void **)dirs);
	if (is_double_slash(path))
		add_double_slash(&result);
	return (result);
}
