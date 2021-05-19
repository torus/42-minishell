#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

int	g_success_count = 0;
int	g_fail_count = 0;

bool	test_check(int64_t val, const char *msg)
{
	if (val)
	{
		g_success_count++;
		printf("  ✔ %s\n", msg);
		return (true);
	}
	else
	{
		g_fail_count++;
		printf("  ✖ %s\n", msg);
		return (false);
	}
}

int	print_result()
{
	printf("✔: %d ✖: %d\n", g_success_count, g_fail_count);
	return (g_fail_count);
}
