#include "utils.h"
#include "../libft/libft.h"

/*
 * first に second を繋げて, firstだけfreeする
 */
char	*strjoin_and_free_first(char *first, char *second)
{
	char	*result;

	result = ft_strjoin(first, second);
	free(first);
	return (result);
}

/*
 * first に second を繋げて, firstとsecondをfreeする
 */
char	*strjoin_and_free_both(char *first, char *second)
{
	char	*result;

	result = ft_strjoin(first, second);
	free(first);
	free(second);
	return (result);
}
