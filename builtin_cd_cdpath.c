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

static char	*path_join3(char *path0, char *path1, char *path2)
{
	char	*tmp;
	char	*result;

	tmp = path_join(path0, path1);
	result = path_join(tmp, path2);
	free(tmp);
	return (result);
}

/* sources の各パスを起点として dest_path に移動を試みる.
 *
 * sources: 起点となるパスの文字列の配列
 *
 * Return: 移動が成功したかどうか.
 */
static bool	cd_from_sources(char *dest_path, char **sources)
{
	char	*abs_path;
	int		i;

	i = 0;
	while (sources[i])
	{
		if (sources[i][0] != '/')
			abs_path = path_join3(g_cwd, sources[i], dest_path);
		else
			abs_path = path_join(sources[i], dest_path);
		if (change_directory(abs_path))
		{
			if (ft_strlen(sources[i]))
				printf("%s\n", g_cwd);
			free(abs_path);
			return (true);
		}
		free(abs_path);
		i++;
	}
	return (false);
}

/* $CDPATH を元に移動する.
 *
 * dest_path: 移動先相対パス
 * Return: 移動に成功したら true を返す.
 *   そうでない場合は false を返す.
 */
int	cd_cdpath_env(char *dest_path)
{
	char	*cdpath;
	char	**sources;
	bool	status;

	cdpath = get_env_val("CDPATH");
	if (!cdpath)
		return (false);
	sources = get_colon_units(cdpath);
	status = cd_from_sources(dest_path, sources);
	free(cdpath);
	free_ptrarr((void **)sources);
	return (status);
}
