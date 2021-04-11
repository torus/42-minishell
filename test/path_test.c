#include "test.h"
#include "../execution.h"

int main(){
	TEST_SECTION("path_join() 最後スラッシュ無し");
	{
		char *fullpath = path_join("/usr/bin", "cat");
		CHECK_EQ_STR(fullpath, "/usr/bin/cat");
		free(fullpath);
	}

	TEST_SECTION("path_join() 最後スラッシュあり");
	{
		char *fullpath = path_join("/usr/bin/", "cat");
		CHECK_EQ_STR(fullpath, "/usr/bin//cat");
		free(fullpath);
	}

	int fail_count = print_result();
	return (fail_count);
}
