#include "libft.h"

/* return false if error has occured */
bool	ft_atol(const char *nptr, long *result)
{
	long	num;
	long	sign;

	sign = 1;
	num = 0;
	while (*nptr == ' ' || *nptr == '\t' || *nptr == '\f'
		|| *nptr == '\r' || *nptr == '\n' || *nptr == '\v')
		nptr++;
	if (*nptr == '-')
	{
		sign = -1;
		nptr++;
	}
	if (!ft_isdigit(*nptr) || is_long_overflow(nptr, sign))
		return (false);
	while (ft_isdigit(*nptr))
		num = num * 10 + (*nptr++ - '0');
	while (*nptr == ' ' || *nptr == '\t' || *nptr == '\f'
		|| *nptr == '\r' || *nptr == '\n' || *nptr == '\v')
		nptr++;
	if (*nptr)
		return (false);
	if (result)
		*result = sign * num;
	return (true);
}
