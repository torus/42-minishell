#include "env.h"
#include "minishell.h"

/*
 * コマンドの終了ステータスを取得する
 */
int	get_status(void)
{
	return (g_shell.status);
}

/*
 * コマンドの終了ステータスを設定する
 */
void	set_status(int status_value)
{
	g_shell.status = status_value;
}

int	set_status_and_ret(int status_value, int ret_value)
{
	g_shell.status = status_value;
	return (ret_value);
}
