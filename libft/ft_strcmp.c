#include "libft.h"

int	ft_strcmp(const char *s1, const char *s2)
{
	while (*s1 || *s2)
	{
		if (*s1 != *s2)
		{
			if (*s1 > *s2)
				return (1);
			else
				return (-1);
		}
		s1++;
		s2++;
	}
	return (0);
}
