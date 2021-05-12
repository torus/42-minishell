#include "env.h"

/*
 * Get environment variable with key.
 *
 * env_key: key name of environment variable.
 *
 * return: value of environment variable.
 *         That forms "key=value"
 */
char	*get_env(const char *env_key)
{
	size_t		idx;
	char		**key_val;
	extern char	**environ;

	idx = 0;
	while (environ[idx])
	{
		key_val = ft_split(environ[idx], '=');
		if (!key_val)
			return (NULL);
		if (ft_strncmp(key_val[0], env_key, ft_strlen(env_key) + 1) == 0)
		{
			free_ptrarr((void **)key_val);
			break ;
		}
		free_ptrarr((void **)key_val);
		idx++;
	}
	return (environ[idx]);
}

/*
 * Split a string at the first appearance of a character.
 *
 * str: zero-terminated string.
 * c: split text with this character.
 *
 * return: NULL-terminated char's pointer array.
 */
char	**split_first_c(const char *str, char c)
{
	size_t	idx;
	char	**result;

	result = ft_calloc(3, sizeof(char *));
	if (!result)
		return (NULL);
	idx = 0;
	while (true)
	{
		if (str[idx] == c || str[idx] == '\0')
		{
			result[0] = ft_substr(str, 0, idx);
			if (!result[0])
				return (free_ptrarr_and_rtn_null((void **)result));
			break ;
		}
		idx++;
	}
	if (str[idx])
	{
		result[1] = ft_strdup(str + idx + 1);
		if (!result[1])
			return (free_ptrarr_and_rtn_null((void **)result));
	}
	return (result);
}

/* コロン(:)で区切る.
 * ft_split() と違い, "::/" のような入力に対して
 * ["", "", "/"] を作成して返す.
 *
 * str: コロン(':')区切られた文字列
 * def_str: default string.
 */
char	**get_colon_units(char *str, char *default_str)
{
	void	**result;
	char	*next;
	void	**tmparr;

	result = NULL;
	next = ft_strchr(str, ':');
	while (next)
	{
		tmparr = result;
		if (next - str == 0)
			result = ptrarr_add_ptr(result, ft_strdup(default_str));
		else
			result = ptrarr_add_ptr(result, ft_substr(str, 0, next - str));
		free(tmparr);
		str = next + 1;
		next = ft_strchr(str, ':');
	}
	tmparr = result;
	if (*str == '\0')
		result = ptrarr_add_ptr(result, ft_strdup(default_str));
	else
		result = ptrarr_add_ptr(result, ft_strdup(str));
	free(tmparr);
	return ((char **)result);
}

/*
 * Get value from key-value string.
 * key-value string form like "key=value", "key:value", or something like that.
 *
 * kvstr: key-value string.
 * delimiter: delimiter of key-value string.
 *
 * return: value of key-value string.
 */
char	*get_val_from_kvstr(const char *kvstr, char delimiter)
{
	char	**kvarr;
	char	*valstr;

	kvarr = split_first_c(kvstr, delimiter);
	if (!kvarr)
		return (NULL);
	valstr = kvarr[1];
	free(kvarr[0]);
	free(kvarr);
	return (valstr);
}

/*
 * Get value from environment variable with key ("key=value").
 *
 * env_key: key name of environment variable
 *
 * return: value of environment variable if env_key is found,
 *         or else, return NULL.
 */
char	*get_env_val(char *env_key)
{
	char	*path_env_kv;
	char	*path_env_val;

	if (ft_strncmp(env_key, "?", 2) == 0)
		return (ft_itoa(get_status()));
	path_env_kv = get_env(env_key);
	if (!path_env_kv)
		return (NULL);
	path_env_val = get_val_from_kvstr(path_env_kv, '=');
	if (!path_env_val)
		return (NULL);
	return (path_env_val);
}
