#include "env.h"

static int	*get_status_ptr()
{
	static int	status;

	return (&status);
}

/*
 * コマンドの終了ステータスを取得する
 */
int	get_status()
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
