#include "builtin.h"
#include "env.h"

int	builtin_cd(char **argv)
{
	char	*dest_path;

	if (ptrarr_len((void **)argv) > 2)
		return (put_err_msg_and_ret("too many arguments"));
	if (ptrarr_len((void **)argv) == 2)
		dest_path = ft_strdup(argv[1]);
	else
		dest_path = get_env_val("HOME");
	if (chdir(dest_path) < 0)
		return (put_err_msg_and_ret("No such file or directory"));
	return (0);
}
