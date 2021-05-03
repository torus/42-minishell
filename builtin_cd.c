#include "builtin.h"
#include "env.h"
#include "minishell.h"

static int	put_cd_errmsg_and_ret(char **argv)
{
	char *errmsg;

	// エラーメッセージの生成
	errmsg = NULL;
	if (errno == ENOENT)
		errmsg = ft_strjoin(argv[1], ": No such file or directory");
	else if (errno == ENOTDIR)
		errmsg = ft_strjoin(argv[1], ": Not a directory");
	else if (errno == EACCES)
		errmsg = ft_strjoin(argv[1], ": Permission denied");

	if (!errmsg)
		put_minish_err_msg_and_exit(1, argv[0], "generating errmsg is failed!");
	put_minish_err_msg(argv[0], errmsg);
	free(errmsg);
	return (1);
}

int	builtin_cd(char **argv)
{
	char	*dest_path;
	char	chdir_status;

	if (ptrarr_len((void **)argv) > 2)
		return (put_minish_err_msg_and_ret(1, argv[0], "too many arguments"));
	if (ptrarr_len((void **)argv) == 2)
		dest_path = ft_strdup(argv[1]);
	else
		dest_path = get_env_val("HOME");
	chdir_status = chdir(dest_path);
	free(dest_path);
	if (chdir_status < 0)
		return (put_cd_errmsg_and_ret(argv));
	return (0);
}
