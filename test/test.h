#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#define CHECK(val) test_check((int64_t)val, #val)
#define CHECK_EQ(actual, expected) test_check(actual == expected, #actual " == " #expected)
#define CHECK_EQ_STR(actual, expected) \
  do { \
	test_check(strcmp(actual, expected) == 0, #actual " == " #expected); \
	printf("    |%s| == |%s|\n", actual, expected); \
  } while(0);
#define TEST_CHAPTER(message) printf("#\n# " message "\n#\n")
#define TEST_SECTION(message) printf("- " message "\n")

bool	test_check(int64_t val, const char *msg);
int	print_result();

extern int	g_success_count;
extern int	g_fail_count;
