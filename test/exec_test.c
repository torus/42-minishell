#include "test.h"
#include "../execution.h"
#include "../minishell.h"

t_shell	g_shell;

int main(){
	init_g_shell();

	TEST_SECTION("find_executable_file_in_dir() 実行可能ファイルが存在する");
	{
		char *fullpath = find_executable_file_in_dir("cat", "/usr/bin");
		CHECK_EQ_STR(fullpath, "/usr/bin/cat");
		free(fullpath);
	}

	TEST_SECTION("find_executable_file_in_dir() 実行可能ファイルは存在するがdirpathの最後にスラッシュが付いている");
	{
		char *fullpath = find_executable_file_in_dir("cat", "/usr/bin/");
		CHECK_EQ_STR(fullpath, "/usr/bin/cat");
		free(fullpath);
	}

	TEST_SECTION("find_executable_file_in_dir() 実行可能ファイルが見つからない時"); {
		char *fullpath = find_executable_file_in_dir("cat", "/");
		CHECK_EQ(fullpath, NULL);
		free(fullpath);
	}

	int fail_count = print_result();
	return (fail_count);
}
