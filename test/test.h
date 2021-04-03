#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define CHECK(val) test_check((int64_t)val, #val)
#define CHECK_EQ(actual, expected) test_check(actual == expected, #actual " == " #expected)
#define TEST_SECTION(message) printf("- " message "\n")

void	test_check(int64_t val, const char *msg);
int	print_result();

extern int	g_success_count;
extern int	g_fail_count;
