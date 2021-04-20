#include "libft.h"

size_t	ptrarr_len(void **ptrarr)
{
	size_t	i;

	i = 0;
	while (ptrarr[i])
		i++;
	return (i);
}

void	free_ptrarr(void **ptrarr)
{
	size_t	i;

	i = 0;
	while (ptrarr[i])
	{
		free(ptrarr[i]);
		ptrarr[i] = NULL;
		i++;
	}
	free(ptrarr);
}

void	*free_ptrarr_and_rtn_null(void **ptrarr)
{
	free_ptrarr(ptrarr);
	return (NULL);
}

void	free_ptrarr_and_assign_null(void ***ptrarr)
{
	free_ptrarr(*ptrarr);
	*ptrarr = NULL;
}

/*
** strarr add str みたいに使う
*/
void	**ptrarr_add_ptr(void **ptrarr, void *ptr)
{
	size_t	arr_size;
	void	**new_ptrarr;

	arr_size = 0;
	if (ptrarr)
		arr_size = ptrarr_len(ptrarr);
	new_ptrarr = ft_calloc(arr_size + 2, sizeof(void *));
	if (!new_ptrarr)
		return (NULL);
	ft_memcpy(new_ptrarr, ptrarr, arr_size);
	if (ptrarr)
		free_ptrarr(ptrarr);
	new_ptrarr[arr_size] = ptr;
	return (new_ptrarr);
}
