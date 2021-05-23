#include "libft.h"

static void	swap_strarr(char **strarr, int i, int j)
{
	char	*tmp;

	tmp = strarr[i];
	strarr[i] = strarr[j];
	strarr[j] = tmp;
}

static int	partition_strarr(char **strarr, int p, int r)
{
	int		i;
	int		j;

	i = p - 1;
	j = p;
	while (j < r)
	{
		if (ft_strcmp(strarr[j], strarr[r]) < 0)
		{
			i++;
			swap_strarr(strarr, i, j);
		}
		j++;
	}
	swap_strarr(strarr, i + 1, r);
	return (i + 1);
}

static void	quick_sort_strarr(char **strarr, int p, int r)
{
	int	q;
	if (p < r)
	{
		q = partition_strarr(strarr, p, r);
		quick_sort_strarr(strarr, p, q - 1);
		quick_sort_strarr(strarr, q + 1, r);
	}
}

void	sort_strarr(char **strarr)
{
	if (!strarr || !ptrarr_len((void **)strarr))
		return ;
	quick_sort_strarr(strarr, 0, ptrarr_len((void **)strarr) - 1);
}
