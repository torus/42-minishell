#include <stdint.h>
#include <stdio.h>

int	g_success_count = 0;
int	g_fail_count = 0;

void	test_check(int64_t val, const char *msg)
{
	if (val)
	{
		g_success_count++;
		printf("  ✔ %s\n", msg);
	}
	else
	{
		g_fail_count++;
		printf("  ✖ %s\n", msg);
	}
}

int	print_result()
{
	printf("✔: %d ✖: %d\n", g_success_count, g_fail_count);
	return (g_fail_count);
}
