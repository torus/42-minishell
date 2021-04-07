#include "env.h"

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

char	*get_val_from_kvstr(const char *kvstr, char key)
{
	char	**kvarr;
	char	*valstr;

	kvarr = split_first_c(kvstr, key);
	if (!kvarr)
		return (NULL);
	valstr = kvarr[1];
	free(kvarr[0]);
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
