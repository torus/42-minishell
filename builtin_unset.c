#include "builtin.h"

/*
 * builtin unset command
 * argv: ["unset", "ABC", "DEF"]
 */
int	builtin_unset(char **argv)
{
	argv++;
	while (*argv)
	{
		ft_unsetenv(*argv);
		argv++;
	}
	return (0);
}
