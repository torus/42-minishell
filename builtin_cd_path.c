#include "path.h"
#include "env.h"
#include "minishell.h"
#include <string.h>

static void	update_path(const char **path, const char *retrieve_path)
{
	const char	*tmp;

	if (ft_strncmp(retrieve_path, ".", 2) == 0)
		return ;
	else if (ft_strncmp(retrieve_path, "..", 3) == 0)
	{
		tmp = *path;
		*path = get_parent_dir(*path);
		free((void *)tmp);
	}
	else
	{
		tmp = *path;
		*path = path_join(*path, retrieve_path);
		free((void *)tmp);
	}
}

static bool	is_double_slash(const char *path)
{
	return (ft_strlen(path) >= 2
		&& path[0] == '/' && path[1] == '/' && path[2] != '/');
}

static void	add_double_slash(const char **path)
{
	const char	*tmp;

	tmp = *path;
	*path = ft_strjoin("/", *path);
	free((void *)tmp);
}

/* パスを正規化する.
 * ex:
 *   - "////hoge//fuga//../././//gaga" -> "/hoge/gaga"
 */
const char	*canonicalize_path(const char *path)
{
	char	**dirs;
	const char	*result;
	int		i;

	dirs = ft_split(path, '/');
	i = 0;
	result = ft_strdup("/");
	while (dirs[i])
	{
		update_path(&result, dirs[i]);
		if (!result || !is_directory(result))
		{
			free((void *)result);
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
