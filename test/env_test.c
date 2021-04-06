#include "test.h"
#include "../env.h"
int main(){
    TEST_SECTION("split_first_c()");
	{
		char *input = "PATH=/bin/:/usr/bin/:/home/jun/bin";
		char **str_arr = split_first_c(input, '=');
		CHECK_EQ_STR(str_arr[0], "PATH");
		CHECK_EQ_STR(str_arr[1], "/bin/:/usr/bin/:/home/jun/bin");
	}

    TEST_SECTION("get_env()");
    {
		
    }

	int fail_count = print_result();
	return (fail_count);
}
