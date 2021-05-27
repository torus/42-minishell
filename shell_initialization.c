#include <unistd.h>
#include "minishell.h"
#include "env.h"
#include "libft/libft.h"

static void	init_pwd(void)
{
	t_var	*pwd_var;
	char	*tmp;

	pwd_var = get_env("PWD");
	if (!pwd_var || !pwd_var->value)
	{
		tmp = getcwd(NULL, 0);
		if (tmp)
			ft_setenv("PWD", tmp, 1);
		free(tmp);
	}
}

static void	init_shlvl(void)
{
	t_var	*shlvl_var;
	char	*num_str;
	int		num;

	shlvl_var = get_env("SHLVL");
	if (shlvl_var && shlvl_var->value)
	{
		num = ft_atoi(shlvl_var->value);
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
}

void	init_g_shell(void)
{
	extern char **environ;

	g_shell.cwd = NULL;
	g_shell.vars = environ2vars(environ);
	g_shell.status = 0;
}

/* シェルの環境を初期化する */
int	initialize_shell(void)
{
	extern char	**environ;

	init_g_shell();
	set_shell_sighandlers();
	sort_strarr(environ);
	init_pwd();
	init_shlvl();
	return (0);
}
