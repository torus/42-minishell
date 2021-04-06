#include "env.h"

char	*get_env(const char *env_key)
{
	size_t	idx;
	char	**key_val;

	idx = 0;
	while (__environ[idx])
	{
		key_val = ft_split(__environ[idx], '=');
		if (!key_val)
			return (NULL);
		if (ft_strncmp(key_val[0], env_key, ft_strlen(env_key)))
		{
			free_ptrarr((void **)key_val);
			break ;
		}
		free_ptrarr((void **)key_val);
		idx++;
	}
	return (__environ[idx]);
}

char	**split_first_c(const char *str, char c)
{
	size_t	idx;
	char	**result;

	result = ft_calloc(3, sizeof(char *));
	idx = 0;
	while (str[idx])
	{
		if (str[idx] == c)
		{
			result[0] = ft_substr(str, 0, idx);
			if (!result[0])
				return (NULL);
			break;
		}
		idx++;
	}
	result[1] = ft_strdup(str + idx + 1);
	if (!result[1])
		return (NULL);
	return (result);
}

char	*get_val_from_kvstr(const char *kvstr, char key)
{
	char	**kvarr;
	char	*valstr;

	kvarr = split_first_c(kvstr, key);
	if (!kvarr)
		return (NULL);
	valstr = kvarr[1];
	free(kvarr[0]);
	free(kvarr[1]);
	free(kvarr);
	return (valstr);
}

char	*get_env_val(char *env_key)
{
	char	*path_env_kv;
	char	*path_env_val;

	path_env_kv = get_env(env_key);
	if (!path_env_kv)
		return (NULL);
	path_env_val = get_val_from_kvstr(path_env_kv, '=');
	if (!path_env_val)
		return (NULL);
	return (path_env_val);
}
