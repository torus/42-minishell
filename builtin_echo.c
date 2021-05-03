#include "builtin.h"

/*
 * builtin echo command
 * argv: ["echo", "-n", "hello"]
 */
int	builtin_echo(char **argv)
{
	int		len;
	int		i;
	bool	is_n_opt;

	len = ptrarr_len((void **)argv);
	if (len <= 1)
		return (0);
	is_n_opt = ft_strncmp(argv[1], "-n", 3) == 0;
	i = 1 + is_n_opt;
	while (i < len)
	{
		ft_putstr_fd(argv[i], STDOUT_FILENO);
		if (i < len - 1)
			ft_putchar_fd(' ', STDOUT_FILENO);
		i++;
	}
	if (!is_n_opt)
		ft_putchar_fd('\n', STDOUT_FILENO);
	return (0);
}
