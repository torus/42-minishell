#include "env.h"

static int	*get_status_ptr(void)
{
	static int	status;

	return (&status);
}

/*
 * コマンドの終了ステータスを取得する
 */
int	get_status(void)
{
	return (*get_status_ptr());
}

/*
 * コマンドの終了ステータスを設定する
 */
void	set_status(int status_value)
{
	*get_status_ptr() = status_value;
}

int		set_status_and_ret(int status_value, int ret_value)
{
	set_status(status_value);
	return (ret_value);
}
