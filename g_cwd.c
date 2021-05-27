#include <errno.h>
#include <string.h>
#include "path.h"
#include "env.h"
#include "minishell.h"

/* shell.cwd に新しいパスをセットする.
 * chdir() などはしない.
 *
 * 環境変数$PWDが削除されていた場合はシェル変数としてセットする
 */
int	set_cwd(char *abs_path)
{
	char	*oldpwd;
	t_var	*pwd_var;

	pwd_var = get_env("PWD");
	if (!pwd_var || !pwd_var->value)
		oldpwd = ft_strdup("");
	else
		oldpwd = ft_strdup(pwd_var->value);
	free(g_shell.cwd);
	g_shell.cwd = ft_strdup(abs_path);
	ft_setenv("PWD", abs_path, !pwd_var || pwd_var->is_shell_var);
	pwd_var = get_env("OLDPWD");
	ft_setenv("OLDPWD", oldpwd, !pwd_var || pwd_var->is_shell_var);
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
 * "{shell.cwd}/{relative_path}" を返す
 * ex:
 *   - relative_path="dir/dir2/symlink2dir"
 *   - relative_path="./../dir/../././/.//////./"
 */
char	*get_abs_path_from_cwd(char *relative_path)
{
	char	**dirs;
	char	*result;
	char	*tmp;
	int		i;

	result = ft_strdup(g_shell.cwd);
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
