#include "path.h"
#include "env.h"
#include "minishell.h"
#include <string.h>

bool	is_directory(char *path)
{
	struct stat	buf;

	if (stat(path, &buf) != 0)
		return (false);
	if (S_ISDIR(buf.st_mode))
		return (true);
	return (false);
}

/*
 * concatenate dirpath, "/" and filename.
 */
char	*path_join(char *dirpath, char *filename)
{
	char	*tmp;
	char	*result;

	if (ft_strlen(dirpath) == 0 || dirpath[ft_strlen(dirpath) - 1] != '/')
		tmp = ft_strjoin(dirpath, "/");
	else
		tmp = ft_strdup(dirpath);
	if (!tmp)
		return (NULL);
	result = ft_strjoin(tmp, filename);
	free(tmp);
	return (result);
}
