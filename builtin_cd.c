#include <string.h>
#include <errno.h>
#include "builtin.h"
#include "env.h"
#include "path.h"
#include "minishell.h"
#include "libft/libft.h"

static void	put_cd_errmsg(const char *dest_path)
{
	char	*tmp;
	char	*errmsg;

	tmp = ft_strjoin(dest_path, ": ");
	if (!tmp)
		return ;
	errmsg = ft_strjoin(tmp, strerror(errno));
	free(tmp);
	if (!errmsg)
		return ;
	put_minish_err_msg("cd", errmsg);
	free(errmsg);
}

/* Generate destination path from argv
 *
 * argv: arguments of builtin_cd().
 *
 * Return: Destination path of cd command (absolute or relative)
 */
static char	*get_cd_dest_from_argv(char **argv)
{
	t_var	*dest_path_var;

	if (ptrarr_len((void **)argv) == 1)
	{
		dest_path_var = get_env("HOME");
		if (!dest_path_var || !dest_path_var->value)
		{
			put_minish_err_msg("cd", "HOME not set");
			return (NULL);
		}
		return (ft_strdup(dest_path_var->value));
	}
	return (ft_strdup(argv[1]));
}

int	builtin_cd(char **argv)
{
	char	*dest;

	if (!g_shell.cwd)
		g_shell.cwd = getcwd(NULL, 0);
	if (ptrarr_len((void **)argv) > 2)
		return (put_minish_err_msg_and_ret(1, argv[0], "too many arguments"));
	dest = get_cd_dest_from_argv(argv);
	if (!dest)
		return (ERROR);
	if (will_search_cdpath(argv, dest) && cd_cdpath_env(dest))
	{
		free(dest);
		return (0);
	}
	if (change_directory(dest))
	{
		free(dest);
		return (0);
	}
	put_cd_errmsg(dest);
	free(dest);
	return (1);
}
