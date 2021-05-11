#include "builtin.h"
#include "env.h"
#include "path.h"
#include "minishell.h"
#include <string.h>

/* get_cd_dest_from_argv() で取得した移動先パス(絶対or相対) を元に
 * 移動先の絶対パスを返す.
 *
 * dest: get_cd_dest_from_argv() で取得した移動先.
 * is_canon_path: 正規化されたパスかどうか
 *
 * Return: 移動先ディレクトリの絶対パス.
 */
static char	*get_cd_abs_dest(char *dest, bool *is_canon_path)
{
	char	*physical_path;
	char	*canon_path;

	if (dest[0] =='/')
		physical_path = ft_strdup(dest);
	else
		physical_path = path_join(g_cwd, dest);
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

/* cd で移動した先のパスとパス情報を元に
 *   g_cwd をセットする.
 *
 * path: 実際に chdir() が成功したパス.
 * is_canon_path: 正規化されたパスかどうか.
 * is_abs_path: 絶対パスかどうか.
 *
 * Return: ヒープ領域の確保に失敗したら-1,
 *   成功したら0を返す.
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

/* 実際に chdir() を実行して現在のプロセスのcwdを変更する
 *
 * abs_dest: chdir() を実行する絶対パス.
 * arg: chdir(abs_dest) が失敗した時に移動するパス.
 *   これは bash の cd がabs_dest に移動失敗した時に
 *   引数で chdir() を試す挙動に合わせている.
 * is_canon_path: 正規化されたパスかどうか
 *
 * Return: 移動に成功したかどうか.
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

/* プロセスのcwdを変更する
 *
 * dest: get_cd_dest_from_argv() で取得した移動先.
 *
 * Return: 移動に成功したかどうか.
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
