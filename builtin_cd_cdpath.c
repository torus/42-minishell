#include "builtin.h"
#include "env.h"
#include "path.h"
#include "minishell.h"
#include <string.h>

/* $CDPATH を検索するかどうかを返す
 *
 * argv: builtin_cd() の引数.
 * dest: get_cd_dest_from_argv() で取得した移動先パス.
 *
 * Return: $CDPATH を検索する必要があるかどうか.
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

/* $CDPATH を元に移動する.
 *
 * Return: 移動に成功したら true を返す.
 *   そうでない場合は false を返す.
 */
int	cd_cdpath_env(char *dest_path)
{
	char	*cdpath;
	int		i;
	char	*abs_path;
	char	**dirs;
	char	*tmp;

	cdpath = get_env_val("CDPATH");
	if (!cdpath)
		return (false);
	dirs = get_colon_units(cdpath);
	free(cdpath);
	i = 0;
	while (dirs[i])
	{
		if (dirs[i][0] != '/')
		{
			tmp = path_join(g_cwd, dirs[i]);
			abs_path = path_join(tmp, dest_path);
			free(tmp);
		}
		else
			abs_path = path_join(dirs[i], dest_path);
		if (change_directory(abs_path))
		{
			if (ft_strlen(dirs[i]))
				printf("%s\n", g_cwd);
			free(abs_path);
			free_ptrarr((void **)dirs);
			return (true);
		}
		free(abs_path);
		i++;
	}
	free_ptrarr((void **)dirs);
	return (false);
}

