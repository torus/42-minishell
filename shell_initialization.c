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
			ft_setenv("PWD", tmp, 0);
		free(tmp);
	}
	pwd_var = get_env("OLDPWD");
	if (!pwd_var)
		ft_setenv("PWD", NULL, 0);
}

static void	put_shlvl_warnmsg(int shlvl)
{
	char	*num_str;

	num_str = ft_itoa(shlvl);
	if (!num_str)
		put_minish_err_msg_and_exit(1, "initialization", "malloc failed");
	ft_putstr_fd("minishell: warning: shell level (", STDERR_FILENO);
	ft_putstr_fd(num_str, STDERR_FILENO);
	ft_putstr_fd(") too high, resetting to 1\n", STDERR_FILENO);
	free(num_str);
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
		if (num >= 1000)
		{
			put_shlvl_warnmsg(num);
			num = 1;
		}
	}
	else
		num = 1;
	num_str = ft_itoa(num);
	if (num_str)
		ft_setenv("SHLVL", num_str, 0);
	free(num_str);
}

void	init_g_shell(void)
{
	extern char	**environ;

	sort_strarr(environ);
	g_shell.cwd = NULL;
	g_shell.vars = environ2vars(environ);
	g_shell.status = 0;
}

int	initialize_shell(void)
{
	init_g_shell();
	init_pwd();
	init_shlvl();
	return (0);
}
