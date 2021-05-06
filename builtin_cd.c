#include "builtin.h"
#include "env.h"
#include "minishell.h"

char	*g_the_current_directory = NULL;

/* - $OLDPWD に現在の $PWD の値を保存
 * - $PWD に dest_path を保存
 * - g_the_current_directory に dest_path を保存
 */
int	set_working_directory(char *dest_path)
{
}

/* - This is `cd -', equivalent to `cd $OLDPWD'
 *   dest_path == "-" だった場合, $OLDPWD に移動して,
 *   $OLDPWD=$PWD にして, $PWDを更新する.
 *   $OLDPWDに移動失敗した場合は移動しないし, $OLDPWD, $PWD を更新しない.
 * - 絶対パス('/'から始まる)場合は $CDPATH から検索しない
 * - ディレクトリがシンボリックリンクだった場合, chdir(dest_path)して,
 *   the_current_directory=dest_path にする.
 */
static int	change_directory(char *dest_path)
{
}

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
	chdir_status = chdir(dest_path);
	if (chdir_status < 0)
		put_cd_errmsg(dest_path);
	free(dest_path);
	if (chdir_status < 0)
		return (1);
	return (0);
}
