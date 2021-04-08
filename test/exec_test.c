#include "test.h"
#include "../execution.h"

extern char **environ;
char *original0;

// 環境変数のテスト用

static void set_env_var(char *env_var)
{
	environ[0] = env_var;
}

static void restore_env_var(void)
{
	environ[0] = original0;
}

int main(){
	// 元の環境変数を保持しておく
	original0 = environ[0];

	TEST_SECTION("find_executable_file_in_dir() 実行可能ファイルが存在する");
	{
		char *fullpath = find_executable_file_in_dir("cat", "/usr/bin");
		CHECK_EQ_STR(fullpath, "/usr/bin/cat");
		free(fullpath);
	}

	TEST_SECTION("find_executable_file_in_dir() 実行可能ファイルは存在するがdirpathの最後にスラッシュが付いている");
	{
		char *fullpath = find_executable_file_in_dir("cat", "/usr/bin/");
		CHECK_EQ_STR(fullpath, "/usr/bin//cat");
		free(fullpath);
	}

	TEST_SECTION("find_executable_file_in_dir() 実行可能ファイルが見つからない時"); {
		char *fullpath = find_executable_file_in_dir("cat", "/");
		CHECK_EQ(fullpath, NULL);
		free(fullpath);
	}

	TEST_SECTION("find_executable_file_from_path_env() PATH内に実行可能ファイルが含まれている");
	{
		set_env_var("PATH=/usr/bin:/home/jun/bin");
		char *fullpath = find_executable_file_from_path_env("cat");
		CHECK_EQ_STR(fullpath, "/usr/bin/cat");
		free(fullpath);
		restore_env_var();
	}

	TEST_SECTION("find_executable_file_from_path_env() PATH内に実行可能ファイルが含まれているが, 最後にスラッシュが付いている");
	{
		set_env_var("PATH=/usr/bin/:/home/jun/bin");
		char *fullpath = find_executable_file_from_path_env("cat");
		CHECK_EQ_STR(fullpath, "/usr/bin//cat");
		free(fullpath);
		restore_env_var();
	}

	TEST_SECTION("find_executable_file_from_path_env() PATH内に実行可能ファイルが含まれていない");
	{
		set_env_var("PATH=/usr/bin/:/home/jun/bin");
		char *fullpath = find_executable_file_from_path_env("not_exists");
		CHECK_EQ(fullpath, NULL);
		free(fullpath);
		restore_env_var();
	}

	TEST_SECTION("find_executable_file_from_path_env() CWD内にシェルスクリプトが含まれている");
	{
		set_env_var("PATH=/usr/bin/:/home/jun/bin");
		char *fullpath = find_executable_file_from_path_env("fot_test");
		CHECK_TRUE(fullpath != NULL);
		free(fullpath);
		restore_env_var();
	}
}
