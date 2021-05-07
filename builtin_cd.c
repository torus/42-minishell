#include "builtin.h"
#include "env.h"
#include "path.h"
#include "minishell.h"

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
static int	change_to_directory(char *abs_path)
{
	int		status;

	status = chdir(abs_path);
	if (status == 0)
		set_current_working_directory(abs_path);
	return (status);
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

	printf("[debug]\t change_directory() start\n");
	printf("[debug]\t dest_path = |%s|\n", dest_path);
	// $OLDPWD に移動する場合
	if (ft_strncmp(dest_path, "-", 2) == 0)
	{
		abs_path = get_env_val("OLDPWD");
		printf("[debug]\t change to $OLDPWD start\n");
		printf("[debug]\t $OLDPWD = |%s|\n", abs_path);
		status = change_to_directory(abs_path);
		printf("[debug]\t change_to_directory() = %d\n", status);
		free(abs_path);
		if (status == -1)
			put_cd_errmsg(abs_path);
		return (status);
	}
	if (dest_path[0] == '/')
		return (change_to_directory(dest_path));
	// それ以外の場合は, 現在のディレクトリからdest_pathに移動出来ればOK.
	// TODO: get_abs_path_from_cwd(dest_path) で取得した絶対パスに chdir() する.
	abs_path = get_abs_path_from_cwd(dest_path);
	printf("[debug]\t abs_path = |%s|\n", abs_path);
	status = change_to_directory(abs_path);
	free(abs_path);
	if (status == 0)
		return (0);
	/*
	// 絶対パスが渡された場合 $CDPATH から検索しない
	// そうでなければ $CDPATH を起点として移動してみる.
	if (dest_path[0] != '/')
	{

	}
	// TODO: $CDPATH から試していって, うまくいけばok. だめならreturn(1);
	// TODO: ここまで全て失敗したら chdir(dest_path) を試す.
	*/
	put_cd_errmsg(dest_path);
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
	if (chdir_status < 0)
		put_cd_errmsg(dest_path);
	free(dest_path);
	if (chdir_status < 0)
		return (1);
	return (0);
}

int	builtin_cd(char **argv)
{
	char	*dest_path;
	int		chdir_status;

	if (ptrarr_len((void **)argv) > 2)
		return (put_minish_err_msg_and_ret(1, argv[0], "too many arguments"));
	if (ptrarr_len((void **)argv) == 2)
		dest_path = ft_strdup(argv[1]);
	else
		return (cd_home());
	chdir_status = change_directory(dest_path);
	if (chdir_status < 0)
		put_cd_errmsg(dest_path);
	free(dest_path);
	if (chdir_status < 0)
		return (1);
	return (0);
}
