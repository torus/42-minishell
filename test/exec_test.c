#include "test.h"
#include "../execution.h"

int main(){
	TEST_SECTION("find_executable_file_in_dir(\"cat\", \"/usr/bin\")");
	{
		char *fullpath = find_executable_file_in_dir("cat", "/usr/bin");
		CHECK_EQ_STR(fullpath, "/usr/bin/cat");
		free(fullpath);
	}

	TEST_SECTION("find_executable_file_in_dir(\"cat\", \"/usr/bin/\")");
	{
		char *fullpath = find_executable_file_in_dir("cat", "/usr/bin/");
		CHECK_EQ_STR(fullpath, "/usr/bin/cat");
		free(fullpath);
	}

	TEST_SECTION("find_executable_file_in_dir(\"cat\", \"/\")");
	{
		char *fullpath = find_executable_file_in_dir("cat", "/usr/bin/");
		CHECK_EQ(fullpath, NULL);
		free(fullpath);
	}
}
