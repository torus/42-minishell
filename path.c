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
