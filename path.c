#include "libft/libft.h"

char	*path_join(char *dirpath, char *filename)
{
	char	*tmp;
	char	*result;

	tmp = ft_strjoin(dirpath, "/");
	if (!tmp)
		return (NULL);
	result = ft_strjoin(tmp, filename);
	free(tmp);
	return (result);
}
