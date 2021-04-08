#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#define CHECK(val) test_check((int64_t)val, #val)
#define CHECK_EQ(actual, expected) test_check(actual == expected, #actual " == " #expected)
#define CHECK_EQ_STR(actual, expected) test_check(strcmp(actual, expected) == 0, #actual " == " #expected)
#define CHECK_TRUE(actual) test_check(actual, #actual " == true")
#define CHECK_FALSE(actual) test_check(actual, #actual " == false")
#define TEST_SECTION(message) printf("- " message "\n")

void	test_check(int64_t val, const char *msg);
int	print_result();

extern int	g_success_count;
extern int	g_fail_count;
