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

