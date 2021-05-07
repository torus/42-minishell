#include "path.h"
#include "env.h"

char	*g_cwd;

/* g_cwd に新しいパスをセットする.
 * chdir() などはしない.
 */
int	set_current_working_directory(char *abs_path)
{
	char	*oldpwd;

	oldpwd = get_env_val("PWD");
	free(g_cwd);
	g_cwd = ft_strdup(abs_path);
	ft_setenv("OLDPWD", oldpwd, 1);
	ft_setenv("PWD", abs_path, 1);
	free(oldpwd);
	return (0);
}

/* 絶対パスを構築する
 * "{g_cwd}/{relative_path}" を返す
 * ex:
 *   - relative_path="dir/dir2/symlink2dir"
 *   - relative_path="./../dir/../././/.//////./"
 */
char *get_abs_path_from_cwd(char *relative_path)
{
	char	**directions;  // ("..", ".", その他) みたいなのが入ってくる
	char	*current_dir;
	char	*tmp;
	int		idx;

	if (!g_cwd)
		g_cwd = getcwd(NULL, 0);
	current_dir = ft_strdup(g_cwd);
	directions = ft_split(relative_path, '/');
	if (!directions)
		return (NULL);
	idx = 0;
	while (directions[idx])
	{
		if (ft_strncmp(current_dir, "/", 2) != 0
			&& ft_strncmp(directions[idx], "..", 3) == 0)
		{
			tmp = current_dir;
			current_dir = ft_substr(tmp, 0, ft_strrchr(tmp, '/') - tmp);
			free(tmp);
		}
		else if ((ft_strncmp(current_dir, "/", 2) == 0
				&& ft_strncmp(directions[idx], "..", 3) == 0)
			|| ft_strncmp(directions[idx], ".", 2) == 0)
			;
		else
		{
			tmp = current_dir;
			current_dir = path_join(tmp, directions[idx]);
			free(tmp);
		}
		idx++;
	}
	free_ptrarr((void **)directions);
	return (current_dir);
}

/* パスを正規化する.
 * ex:
 *   - "////hoge//fuga//../././//gaga" -> "/hoge/gaga"
 */
char	*canonicalize_path(char *path)
{
	char	**dirs;
	char	*result;
	char	*tmp;
	int		i;

	dirs = ft_split(path, '/');
	i = 0;
	result = ft_strdup("/");
	while (dirs[i])
	{
		if (ft_strncmp(dirs[i], "..", 3) == 0)
		{
			// "/" 以外の時, 親ディレクトリに移動する
			if (ft_strncmp(result, "/", 2) != 0)
			{
				tmp = result;
				result = ft_substr(tmp, 0, ft_strrchr(tmp, '/') - tmp);
				free(tmp);
			}
		}
		else if (ft_strncmp(dirs[i], ".", 2) == 0)
			;
		else
		{
			tmp = result;
			result = path_join(tmp, dirs[i]);
			free(tmp);
		}
		i++;
	}
	return (result);
}

/*
 * concatenate dirpath, "/" and filename.
 */
char	*path_join(char *dirpath, char *filename)
{
	char	*tmp;
	char	*result;

	if (dirpath[ft_strlen(dirpath) - 1] != '/')
		tmp = ft_strjoin(dirpath, "/");
	else
		tmp = ft_strdup(dirpath);
	if (!tmp)
		return (NULL);
	result = ft_strjoin(tmp, filename);
	free(tmp);
	return (result);
}
