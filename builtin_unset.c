#include "builtin.h"
#include "minishell.h"

/*
 * unset に渡すキーとして正しいか?
 * 環境変数のキーに設定出来ないやつは駄目という認識で良さそう
 */
bool	is_valid_key(char *key)
{
	int	i;

	i = 0;
	if (!(ft_isalpha(key[i]) || key[i] == '_'))
		return (false);
	while (key[i])
	{
		if (!(ft_isalnum(key[i]) || key[i] == '_'))
			return (false);
		i++;
	}
	return (true);
}

static void	put_unset_errmsg(char *cmd_name, char *keyname)
{
	char	*tmp;
	char	*errmsg;

	tmp = ft_strjoin("`", keyname);
	if (!tmp)
		put_minish_err_msg_and_exit(1, cmd_name, "ft_strjoin() is failed");
	errmsg = ft_strjoin(tmp, "': not a valid identifier");
	free(tmp);
	if (!errmsg)
		put_minish_err_msg_and_exit(1, cmd_name, "ft_strjoin() is failed");
	put_minish_err_msg(cmd_name, errmsg);
	free(errmsg);
}

/*
 * builtin unset command
 * argv: ["unset", "ABC", "DEF"]
 */
int	builtin_unset(char **argv)
{
	int		i;
	bool	is_all_key_ok;

	i = 1;
	is_all_key_ok = true;
	while (argv[i])
	{
		if (is_valid_key(argv[i]))
			ft_unsetenv(argv[i]);
		else
		{
			is_all_key_ok = false;
			put_unset_errmsg(argv[0], argv[i]);
		}
		i++;
	}
	if (!is_all_key_ok)
		return (1);
	return (0);
}
