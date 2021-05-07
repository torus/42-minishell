#include "path.h"
#include "env.h"

char	*g_cwd = NULL;

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
	return (current_dir);
}

/*
 * concatenate dirpath, "/" and filename.
 */
char	*path_join(char *dirpath, char *filename)
{
	char	*tmp;
	char	*result;

	tmp = ft_strjoin(dirpath, "/");
	if (!tmp)
		return (NULL);
	result = ft_strjoin(tmp, filename);
	free(tmp);
	return (result);
}
