#include "env.h"

static bool	is_env_exists(const char *name)
{
	char	*env_val;

	env_val = get_env_val((char *)name);
	free(env_val);
	if (!env_val)
		return (false);
	return (true);
}

/*
 * 標準ライブラリの setenv() と同じ動作をする
 */
int	setenv(const char *name, const char *value, int rewrite)
{
	// 既に環境変数が存在している場合
	//   rewrite=1 なら書き換える.
	//   rewrite=0 なら書き換えない

	// 環境変数が存在していない場合, sizeof(environ) + 1 確保して追加. environの指す先のポインタを変更する
	return (0);
}

/*
 * 標準ライブラリの unsetenv() と同じ動作をする
 */
int	unsetenv(const char *name)
{
	// 環境変数が存在しない場合は何もせずに0を返す
}
