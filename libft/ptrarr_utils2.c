#include "libft.h"

void	**ptrarr_merge(void **ptrarr_first, void **ptrarr_last)
{
	void	**ptrarr_new;
	size_t	first_len;
	size_t	last_len;

	if (!ptrarr_first && !ptrarr_last)
		return (NULL);
	if (!ptrarr_first)
		return (ptrarr_last);
	if (!ptrarr_last)
		return (ptrarr_first);
	first_len = ptrarr_len(ptrarr_first);
	last_len = ptrarr_len(ptrarr_last);
	ptrarr_new = ft_calloc(first_len + last_len + 1, sizeof(void *));
	if (!ptrarr_new)
		return (NULL);
	ft_memcpy(ptrarr_new, ptrarr_first, sizeof(void *) * first_len);
	ft_memcpy(ptrarr_new + first_len, ptrarr_last,
		sizeof(void *) * last_len);
	return (ptrarr_new);
}
