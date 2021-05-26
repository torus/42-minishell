#include <stdlib.h>
#include "libft/libft.h"
#include "env.h"

/*
 * Get environment variable with key.
 *
 * env_key: key name of environment variable.
 *
 * return: found t_var
 */
t_var	*get_env(const char *env_key)
{
	t_var	*current;

	while (current)
	{
		if (!ft_strcmp(env_key, current->key))
			return (current);
		current = current->next;
	}
	return (NULL);
}

/*
 * Split a string at the first appearance of a character.
 *
 * str: zero-terminated string.
 * c: split text with this character.
 *
 * return: NULL-terminated char's pointer array.
 */
const char	**split_first_c(const char *str, char c)
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
	return ((const char **)result);
}

/* コロン(:)で区切る.
 * ft_split() と違い, "::/" のような入力に対して
 * ["", "", "/"] を作成して返す.
 *
 * str: コロン(':')区切られた文字列
 * def_str: default string.
 */
const char	**get_colon_units(const char *str, const char *default_str)
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
	return ((const char **)result);
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
const char	*get_val_from_kvstr(const char *kvstr, char delimiter)
{
	const char	**kvarr;
	const char	*valstr;

	kvarr = split_first_c(kvstr, delimiter);
	if (!kvarr)
		return (NULL);
	valstr = kvarr[1];
	free((void *)kvarr[0]);
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
const char	*get_env_val(const char *env_key)
{
	char	*path_env_kv;
	char	*path_env_val;

	if (ft_strncmp(env_key, "?", 2) == 0)
		return (ft_itoa(get_status()));
	t_var	*var;
	var = get_env(env_key);
	if (!var)
		return (NULL);
	return (var->value);
}
