#include "path.h"
#include "env.h"
#include "minishell.h"
#include <string.h>

char	*g_cwd;

/* g_cwd に新しいパスをセットする.
 * chdir() などはしない.
 */
int	set_cwd(char *abs_path)
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

char	*get_cwd_path(char *caller)
{
	char	*cwd;

	cwd = getcwd(0, 0);
	if (!cwd)
	{
		ft_putstr_fd(caller, STDERR_FILENO);
		ft_putstr_fd(": ", STDERR_FILENO);
		ft_putstr_fd("error retrieving current directory", STDERR_FILENO);
		ft_putstr_fd(": ", STDERR_FILENO);
		ft_putstr_fd("getcwd: cannot access parent directories", STDERR_FILENO);
		ft_putstr_fd(": ", STDERR_FILENO);
		ft_putstr_fd(strerror(errno), STDERR_FILENO);
		ft_putstr_fd("\n", STDERR_FILENO);
	}
	return (cwd);
}

void	put_cwd_err_msg(char *for_whom)
{
	write(STDERR_FILENO, for_whom, ft_strlen(for_whom));
	write(STDERR_FILENO, ": error retrieving current directory: "
		"getcwd: cannot access parent directories: "
		"No such file or directory\n", 106);
}

/* 絶対パスを構築する
 * "{g_cwd}/{relative_path}" を返す
 * ex:
 *   - relative_path="dir/dir2/symlink2dir"
 *   - relative_path="./../dir/../././/.//////./"
 */
char *get_abs_path_from_cwd(char *relative_path)
{
	char	**dirs;  // ("..", ".", その他) みたいなのが入ってくる
	char	*result;
	char	*tmp;
	int		i;

	result = ft_strdup(g_cwd);
	dirs = ft_split(relative_path, '/');
	if (!dirs)
		return (NULL);
	i = 0;
	while (dirs[i])
	{
		tmp = result;
		result = path_join(tmp, dirs[i]);
		free(tmp);
		i++;
	}
	free_ptrarr((void **)dirs);
	return (result);
}
