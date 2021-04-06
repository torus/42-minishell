#include "test.h"
#include "../env.h"

int main(){
	extern char **environ;

	TEST_SECTION("split_first_c()");
	{
		char *input = "PATH=/bin/:/usr/bin/:/home/jun/bin";
		char **str_arr = split_first_c(input, '=');
		CHECK_EQ_STR(str_arr[0], "PATH");
		CHECK_EQ_STR(str_arr[1], "/bin/:/usr/bin/:/home/jun/bin");
	}

	TEST_SECTION("get_env()");
	{
		char *original0 = environ[0];
		environ[0] = "GET_ENV_TEST=/bin/:/usr/bin/:/home/jun/bin";
		char *kvstr = get_env("GET_ENV_TEST");
		CHECK_EQ_STR(kvstr, environ[0]);
		environ[0] = original0;
	}

	int fail_count = print_result();
	return (fail_count);
}
