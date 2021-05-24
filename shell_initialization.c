#include <unistd.h>
#include "minishell.h"
#include "env.h"
#include "libft/libft.h"

static void	init_pwd(void)
{
	char	*pwd;
	char	*tmp;

	pwd = get_env_val("PWD");
	if (!pwd)
	{
		tmp = getcwd(NULL, 0);
		if (tmp)
			ft_setenv("PWD", tmp, 1);
		free(tmp);
	}
	free(pwd);
}

static void	init_shlvl(void)
{
	char	*shlvl;
	char	*num_str;
	int		num;

	shlvl = get_env_val("SHLVL");
	if (shlvl)
	{
		num = ft_atoi(shlvl);
		if (num < 0)
			num = 0;
		else
			num++;
	}
	else
		num = 1;
	num_str = ft_itoa(num);
	if (num_str)
		ft_setenv("SHLVL", num_str, 1);
	free(num_str);
	free(shlvl);
}

/* シェルの環境を初期化する */
int	initialize_shell(void)
{
	extern char	**environ;

	set_shell_sighandlers();
	sort_strarr(environ);
	init_pwd();
	init_shlvl();
	return (0);
}
