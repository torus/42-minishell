#include "libft.h"

void	free_and_assign_null(void **p)
{
	free(*p);
	*p = NULL;
}

void*	free_and_rtn_ptr(void *p, void *val)
{
	free(p);
	return (val);
}
