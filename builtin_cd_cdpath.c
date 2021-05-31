#include <string.h>
#include <stdio.h>
#include "builtin.h"
#include "env.h"
#include "path.h"
#include "minishell.h"

/* Returns whether you have to search $CDPATH or not
 *
 * argv: argument of builtin_cd().
 * dest: destination path obtained by get_cd_dest_from_argv().
 *
 * Return: whether you have to search $CDPATH or not
 */
bool	will_search_cdpath(char **argv, char *dest)
{
	if (argv[1] == NULL || argv[1][0] == '/')
		return (false);
	if (ft_strcmp((char *)dest, ".") == 0
		|| ft_strcmp((char *)dest, "..") == 0
		|| ft_strncmp((char *)dest, "./", 2) == 0
		|| ft_strncmp((char *)dest, "../", 3) == 0)
		return (false);
	return (true);
}

static char	*path_join3(char *path0, char *path1, char *path2)
{
	char	*tmp;
	char	*result;

	tmp = path_join(path0, path1);
	result = path_join(tmp, path2);
	free(tmp);
	return (result);
}

/* Try to move to dest_path from each path in sources.
 *
 * sources: A string array of paths to start from.
 *
 * Return: Whether chdir() was successful.
 */
static bool	cd_from_sources(char *dest_path, char **sources)
{
	char	*abs_path;
	int		i;

	i = 0;
	while (sources[i])
	{
		if (sources[i][0] != '/')
			abs_path = path_join3(g_shell.cwd, sources[i], dest_path);
		else
			abs_path = path_join(sources[i], dest_path);
		if (change_directory(abs_path))
		{
			if (ft_strlen(sources[i]))
				printf("%s\n", g_shell.cwd);
			free(abs_path);
			return (true);
		}
		free(abs_path);
		i++;
	}
	return (false);
}

/* Try to move to directories that are in $CDPATH.
 *
 * dest_path: relative destination path
 *
 * Return: true if moving directory is successful, otherwise, return false.
 */
int	cd_cdpath_env(char *dest_path)
{
	t_var		*cdpath_var;
	char		**sources;
	bool		status;

	cdpath_var = get_env("CDPATH");
	if (!cdpath_var || !cdpath_var->value)
		return (false);
	sources = get_colon_units(cdpath_var->value, "");
	status = cd_from_sources(dest_path, sources);
	free_ptrarr((void **)sources);
	return (status);
}
