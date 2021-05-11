#include "builtin.h"
#include "env.h"
#include "path.h"
#include "minishell.h"
#include <string.h>

static void	put_cd_errmsg(char *dest_path)
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

/*
 * dest
 * is_canon_path: 正規化されたパスかどうか
 */
static char	*set_cd_path(char *dest, bool *is_canon_path)
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
 * cd_path: 1回目にchdir()を実行するパス
 * arg: 2回目にchdir()を実行するパス
 * is_canon_path: 正規化されたパスかどうか
 *
 * Return: 移動に成功したかどうか.
 */
static bool	change_dir_process(char *cd_path,
	const char *arg, bool is_canon_path)
{
	int	status;
	int	old_errno;

	status = chdir(cd_path);
	if (status == 0)
	{
		set_new_pwd(cd_path, is_canon_path, true);
		return (true);
	}
	old_errno = errno;
	status = chdir(arg);
	if (status == 0)
	{
		set_new_pwd(cd_path, is_canon_path, false);
		return (true);
	}
	errno = old_errno;
	return (false);
}

/* プロセスのcwdを変更する
 *
 * dest: get_cd_dest() で取得した移動先.
 *
 * Return: 移動に成功したかどうか.
 */
static bool	change_directory(char *dest)
{
	char	*path;
	int		status;
	bool	is_canon_path;

	path = set_cd_path(dest, &is_canon_path);
	PRINT_DEBUG("path: |%s|, is_canon_path: %d\n", path, is_canon_path);
	status = change_dir_process(path, dest, is_canon_path);
	free(path);
	return (status);
}

/* cd先のディレクトリをcdコマンドのargvを元に作成して返す.
 *
 * argv: builtin_cd() の引数.
 *
 * Return: cdコマンドの移動先パス(絶対or相対パス).
 */
static char	*get_cd_dest(char **argv)
{
	char	*dest_path;

	if (ptrarr_len((void **)argv) == 1)
	{
		dest_path = get_env_val("HOME");
		if (!dest_path)
		{
			put_minish_err_msg("cd", "HOME not set");
			return (NULL);
		}
		return (dest_path);
	}
	return (ft_strdup(argv[1]));
}

/* $CDPATH を検索するかどうかを返す
 *
 * argv: builtin_cd() の引数.
 * dest: get_cd_dest() で取得した移動先パス.
 *
 * Return: $CDPATH を検索する必要があるかどうか.
 */
static bool	will_search_cdpath(char **argv, char *dest)
{
	if (argv[1] == NULL || argv[1][0] == '/')
		return (false);
	if (ft_strcmp((char *)dest, ".") == 0 ||
		ft_strcmp((char *)dest, "..") == 0 ||
		ft_strncmp((char *)dest, "./", 2) == 0 ||
		ft_strncmp((char *)dest, "../", 3) == 0)
		return (false);
	return (true);
}

/* $CDPATH を元に移動する.
 *
 * Return: 移動に成功したら true を返す.
 *   そうでない場合は false を返す.
 */
static int	cd_cdpath_env(char *dest_path)
{
	char	*cdpath;
	int		i;
	char	*abs_path;
	char	**dirs;
	char	*tmp;

	PRINT_DEBUG("start search dir in $CDPATH\n");
	cdpath = get_env_val("CDPATH");
	if (!cdpath)
		return (false);
	dirs = get_colon_units(cdpath);
	free(cdpath);
	i = 0;
	while (dirs[i])
	{
		PRINT_DEBUG("search in |%s|\n", dirs[i]);
		if (dirs[i][0] != '/')
		{
			tmp = path_join(g_cwd, dirs[i]);
			abs_path = path_join(tmp, dest_path);
			free(tmp);
		}
		else
			abs_path = path_join(dirs[i], dest_path);
		if (change_directory(abs_path))
		{
			if (ft_strlen(dirs[i]))
				printf("%s\n", g_cwd);
			free(abs_path);
			free_ptrarr((void **)dirs);
			return (true);
		}
		free(abs_path);
		i++;
	}
	free_ptrarr((void **)dirs);
	PRINT_DEBUG("dir is not found in $CDPATH\n");
	return (false);
}

int	builtin_cd(char **argv)
{
	char	*dest;

	if (!g_cwd)
		g_cwd = getcwd(NULL, 0);
	if (ptrarr_len((void **)argv) > 2)
		return (put_minish_err_msg_and_ret(1, argv[0], "too many arguments"));
	dest = get_cd_dest(argv);
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
