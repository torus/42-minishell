#include "env.h"

/*
 * keyname, value を元に "key=value" の文字列を作成する
 */
static char	*generate_kvstr(const char *key, const char *value)
{
	char	*tmp;
	char	*kvstr;

	tmp = ft_strjoin(key, "=");
	kvstr = ft_strjoin(tmp, value);
	free(tmp);
	return (kvstr);
}

/*既に環境変数が存在している場合
 *  rewrite=1 なら書き換える.
 *  rewrite=0 なら書き換えない
 */
static int	update_env(const char *name, const char *value, int rewrite)
{
	extern char	**environ;
	int			idx;
	char		*kvstr;

	// 既に環境変数が存在している場合
	//   rewrite=1 なら書き換える.
	//   rewrite=0 なら書き換えない
	idx = 0;
	while (environ[idx])
	{
		if (ft_strcmp(environ[idx], name) == 0)
		{
			if (!rewrite)
				return (0);
			kvstr = generate_kvstr(name, value);
			if (!kvstr)
				return (ERROR);
			environ[idx] = kvstr;
			return (0);
		}
		idx++;
	}
	return (0);
}

/*
 * 標準ライブラリの setenv() と同じ動作をする
 */
int	setenv(const char *name, const char *value, int rewrite)
{
	update_env(name, value, rewrite);  // TODO: update_env() が成功したら return (0) する
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
