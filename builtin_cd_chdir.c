#include <string.h>
#include <errno.h>
#include "builtin.h"
#include "env.h"
#include "path.h"
#include "minishell.h"

/* Returns the absolute path to the destination
 *   based on the destination path (absolute or relative)
 *   obtained by get_cd_dest_from_argv().
 *
 * dest: Destination path (absolute or relative)
 *   obtained by get_cd_dest_from_argv().
 * is_canon_path: Assign whether path is canonicalized to pointer.
 *
 * Return: Absolute path of destination directory.
 */
static char	*get_cd_abs_dest(char *dest, bool *is_canon_path)
{
	char	*physical_path;
	char	*canon_path;

	if (dest[0] == '/')
		physical_path = ft_strdup(dest);
	else
		physical_path = path_join(g_shell.cwd, dest);
	if (!physical_path)
		return (NULL);
	canon_path = canonicalize_path(physical_path);
	if (canon_path)
	{
		free(physical_path);
		*is_canon_path = true;
		return (canon_path);
	}
	free(canon_path);
	*is_canon_path = false;
	return (physical_path);
}

/* Set g_shell.cwd based on the path to the location moved by builtin_cd
 *   and path infomation.
 *
 * path: Path where chdir() succeeded.
 * is_canon_path: Whether the path is canonicalized.
 * is_abs_path: Whether the path is absolute path.
 *
 * Return: Return 0 if success, otherwise, return -1.
 */
static int	set_new_pwd(char *path, bool is_canon_path, bool is_abs_path)
{
	char	*new_cwd;

	new_cwd = NULL;
	if (is_abs_path)
	{
		if (is_canon_path == false)
			new_cwd = get_cwd_path("cd");
		if (is_canon_path || new_cwd == NULL)
			new_cwd = ft_strdup(path);
	}
	else
	{
		new_cwd = get_cwd_path("cd");
		if (!new_cwd)
			new_cwd = ft_strdup(path);
	}
	if (!new_cwd)
		return (ERROR);
	set_cwd(new_cwd);
	free(new_cwd);
	return (SUCCESS);
}

/* Execute chdir() to change cwd of current process.
 *
 * abs_dest: chdir() try to move to this path first.
 * arg: chdir() try to move to this path if chdir(abs_dest) is failed.
 *   This is in line with the behavior of bash's cd,
 *   which tries chdir() with an argument when it fails to move to abs_dest.
 * is_canon_path: Whether the path is canonicalized.
 *
 * Return: true if the chdir() is successful, otherwise, return false.
 */
static bool	change_dir_process(char *abs_dest,
	const char *arg, bool is_canon_path)
{
	int	status;
	int	old_errno;

	status = chdir(abs_dest);
	if (status == 0)
	{
		set_new_pwd(abs_dest, is_canon_path, true);
		return (true);
	}
	old_errno = errno;
	status = chdir(arg);
	if (status == 0)
	{
		set_new_pwd(abs_dest, is_canon_path, false);
		return (true);
	}
	errno = old_errno;
	return (false);
}

/* Change current working directory of current process
 *
 * dest: Destination path obtained by get_cd_dest_from_argv().
 *
 * Return: true if successful, otherwise, return false.
 */
bool	change_directory(char *dest)
{
	char	*path;
	int		status;
	bool	is_canon_path;

	path = get_cd_abs_dest(dest, &is_canon_path);
	status = change_dir_process(path, dest, is_canon_path);
	free(path);
	return (status);
}
