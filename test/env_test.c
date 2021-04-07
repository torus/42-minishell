#include "test.h"
#include "../env.h"

int main(){
	extern char **environ;

	TEST_SECTION("split_first_c() 普通に\"Key=Value\"");
	{
		char *input = "PATH=/bin/:/usr/bin/:/home/jun/bin";
		char **str_arr = split_first_c(input, '=');
		CHECK_EQ_STR(str_arr[0], "PATH");
		CHECK_EQ_STR(str_arr[1], "/bin/:/usr/bin/:/home/jun/bin");
		CHECK_EQ(str_arr[2], NULL);
	}

	TEST_SECTION("split_first_c() 区切り文字が入っていない");
	{
		char *input = "PATH/bin/:/usr/bin/:/home/jun/bin";
		char **str_arr = split_first_c(input, '=');
		CHECK_EQ_STR(str_arr[0], "PATH/bin/:/usr/bin/:/home/jun/bin");
		CHECK_EQ(str_arr[1], NULL);
		CHECK_EQ(str_arr[2], NULL);
	}

	TEST_SECTION("get_env() 通常ケース");
	{
		char *original0 = environ[0];
		environ[0] = "GET_ENV_TEST=/bin/:/usr/bin/:/home/jun/bin";
		char *kvstr = get_env("GET_ENV_TEST");
		CHECK_EQ_STR(kvstr, environ[0]);
		environ[0] = original0;
	}

	TEST_SECTION("get_env() 指定した環境変数が存在しない場合");
	{
		char *kvstr = get_env("THIS_IS_NOT_EXIST_IN_ENVIRON");
		CHECK_EQ(kvstr, NULL);
	}

	TEST_SECTION("get_env() 途中まで同じ環境変数名な時");
	{
		char *original0 = environ[0];
		environ[0] = "GET_ENV_TEST_TEST=/bin/:/usr/bin/:/home/jun/bin";
		char *kvstr = get_env("GET_ENV_TEST");
		CHECK_EQ(kvstr, NULL);
		environ[0] = original0;
	}

	int fail_count = print_result();
	return (fail_count);
}
