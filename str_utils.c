#include "utils.h"
#include "../libft/libft.h"

/*
 * first に second を繋げて, firstだけfreeする
 */
char	*strjoin_and_free_first(char *first, char *second)
{
	char	*tmp;

	tmp = first;
	first = ft_strjoin(first, second);
	free(tmp);
	return (first);
}

