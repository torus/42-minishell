#include "builtin.h"
#include "env.h"
#include "path.h"
#include "minishell.h"

#define DEBUG 1

#if DEBUG == 1
	#define PRINT_DEBUG(fmt, ...) \
		do { \
			printf("[debug] %s:%d:\t" fmt, __FILE__, __LINE__, ##__VA_ARGS__); \
		} while (0);
#else
	#define PRINT_DEBUG(fmt, ...) do {} while(0);
#endif

static void	put_cd_errmsg(char *dest_path)
{
	char	*errmsg;

	errmsg = NULL;
	if (errno == ENOENT)
		errmsg = ft_strjoin(dest_path, ": No such file or directory");
	else if (errno == ENOTDIR)
		errmsg = ft_strjoin(dest_path, ": Not a directory");
	else if (errno == EACCES)
		errmsg = ft_strjoin(dest_path, ": Permission denied");
	if (!errmsg)
		put_minish_err_msg_and_exit(1, "cd", "generating errmsg is failed!");
	put_minish_err_msg("cd", errmsg);
	free(errmsg);
}

/* chdir() で移動が成功したら set_current_working_directory(abs_path) する
 * - ディレクトリがシンボリックリンクだった場合, chdir(dest_path)して,
 *   the_current_directory=dest_path にする.
 *
 * 移動出来たら0, 移動出来なかったら-1を返す
 */
static int	canonicalize_path_and_setcwd(char *abs_path)
{
	char	*canonicalized_path;

	canonicalized_path = canonicalize_path(abs_path);
	PRINT_DEBUG("canonicalized_path: |%s|\n", canonicalized_path);
	set_cwd(canonicalized_path);
	free(canonicalized_path);
	return (0);
}

/* $CDPATH から移動先ディレクトリを探す
 */
static int cd_cdpath(char *dest_path)
{
	char	*cdpath_env;
	int		i;
	char	*abs_path;
	char	**dirs;
	int		status;

	PRINT_DEBUG("start search dir in $CDPATH\n");
	cdpath_env = get_env_val("CDPATH");
	if (!cdpath_env)
		return (1);
	dirs = ft_split(cdpath_env, ':');
	free(cdpath_env);
	i = 0;
	while (dirs[i])
	{
		PRINT_DEBUG("search in |%s|\n", dirs[i]);
		if (dirs[i][0] != '/')
		{
			char *tmp = path_join(g_cwd, dirs[i]);
			abs_path = path_join(tmp, dest_path);
			free(tmp);
		}
		else
			abs_path = path_join(dirs[i], dest_path);
		status = chdir(abs_path);
		if (status == 0)
		{
			PRINT_DEBUG("dir is found!!\n");
			PRINT_DEBUG("abs_path = |%s|\n", abs_path);
			canonicalize_path_and_setcwd(abs_path);
			free(abs_path);
			free_ptrarr((void **)dirs);
			printf("%s\n", g_cwd);
			return (0);
		}
		free(abs_path);
		i++;
	}
	free_ptrarr((void **)dirs);
	PRINT_DEBUG("dir is not found in $CDPATH\n");
	return (1);
}

/* - This is `cd -', equivalent to `cd $OLDPWD'
 *   dest_path == "-" だった場合, $OLDPWD に移動して,
 *   $OLDPWD=$PWD にして, $PWDを更新する.
 *   $OLDPWDに移動失敗した場合は移動しないし, $OLDPWD, $PWD を更新しない.
 * - 絶対パス('/'から始まる)の場合は $CDPATH から検索しない
 * - $CDPATHから見つかった場合は移動先の絶対パスを標準出力する
 */
static int	change_directory(char *dest_path)
{
	char	*abs_path;
	int		status;

	PRINT_DEBUG("change_directory() start\n");
	PRINT_DEBUG("dest_path = |%s|\n", dest_path);
	// $OLDPWD に移動する場合
	if (ft_strncmp(dest_path, "-", 2) == 0)
	{
		abs_path = get_env_val("OLDPWD");
			PRINT_DEBUG("change to $OLDPWD start\n");
			PRINT_DEBUG("$OLDPWD = |%s|\n", abs_path);

		status = chdir(abs_path);
			PRINT_DEBUG("chdir() = %d\n", status);

		if (status == 0)
			canonicalize_path_and_setcwd(abs_path);
		else
			put_cd_errmsg(abs_path);

		free(abs_path);
		return (status);
	}
	// 絶対パスの場合
	else if (dest_path[0] == '/')
	{
		status = chdir(dest_path);
		if (status == 0)
			canonicalize_path_and_setcwd(dest_path);
		else
			put_cd_errmsg(dest_path);
		return (status);
	}
	// 相対パスの場合
	else
	{
		// bashの仕様では, $CDPATH を調べてからカレントディレクトリを調べる
		// ".", "./*" の時は $CDPATH を探索しない
		if ((ft_strncmp(dest_path, ".", 2) && ft_strncmp(dest_path, "./", 2))
		&& cd_cdpath(dest_path) == 0)
			return (0);

		// それ以外の場合は, 現在のディレクトリとdest_pathを繋げて正規化したパスに移動出来ればOK.
		// get_abs_path_from_cwd(dest_path) で取得した絶対パスに chdir() する.
		abs_path = get_abs_path_from_cwd(dest_path);
		status = chdir(abs_path);
		PRINT_DEBUG("chdir(|%s|) = %d\n", abs_path, status);
		if (status == 0)
		{
			PRINT_DEBUG("abs_path = |%s|\n", abs_path);
			canonicalize_path_and_setcwd(abs_path);
			free(abs_path);
			return (0);
		}
		free(abs_path);

		// ここまで全て失敗したら chdir(dest_path) を試す.
		status = chdir(dest_path);
		PRINT_DEBUG("chdir(|%s|) = %d\n", dest_path, status);
		if (status == 0)
		{
			abs_path = path_join(g_cwd, dest_path);
			PRINT_DEBUG("abs_path = |%s|\n", abs_path);
			set_cwd(abs_path);
			free(abs_path);
			return (0);
		}
	}
	return (1);
}

static int	cd_home(void)
{
	char	*dest_path;
	int		chdir_status;

	dest_path = get_env_val("HOME");
	if (!dest_path)
		return (put_minish_err_msg_and_ret(1, "cd", "HOME not set"));
	chdir_status = 0;
	if (ft_strlen(dest_path))
		chdir_status = chdir(dest_path);
	if (chdir_status == 0)
	{
		char *abs_path;
		if (dest_path[0] == '/')
			abs_path = ft_strdup(dest_path);
		else
			abs_path = path_join(g_cwd, dest_path);
		canonicalize_path_and_setcwd(abs_path);
		free(abs_path);
	}
	else
		put_cd_errmsg(dest_path);
	free(dest_path);
	if (chdir_status < 0)
		return (1);
	return (0);
}

int	builtin_cd(char **argv)
{
	int		chdir_status;

	if (!g_cwd)
		g_cwd = getcwd(NULL, 0);
	if (ptrarr_len((void **)argv) > 2)
		return (put_minish_err_msg_and_ret(1, argv[0], "too many arguments"));
	else if (ptrarr_len((void **)argv) == 1)
		return (cd_home());
	chdir_status = change_directory(argv[1]);
	if (chdir_status != 0)
		put_cd_errmsg(argv[1]);
	if (chdir_status != 0)
		return (1);
	return (0);
}
