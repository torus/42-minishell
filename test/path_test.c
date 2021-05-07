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
		CHECK_EQ_STR(fullpath, "/usr/bin/cat");
		free(fullpath);
	}

	TEST_SECTION("canonicalize_path() /");
	{
		char *input = "/";
		char *expected = "/";
		char *actual = canonicalize_path(input);
		CHECK_EQ_STR(actual, expected);
		free(actual);
	}

	TEST_SECTION("canonicalize_path() /../..");
	{
		char *input = "/../..";
		char *expected = "/";
		char *actual = canonicalize_path(input);
		CHECK_EQ_STR(actual, expected);
		free(actual);
	}

	TEST_SECTION("canonicalize_path() ///////hoge/////fuga");
	{
		char *input = "///////hoge/////fuga";
		char *expected = "/hoge/fuga";
		char *actual = canonicalize_path(input);
		CHECK_EQ_STR(actual, expected);
		free(actual);
	}

	TEST_SECTION("canonicalize_path() /../../../..////////hoge/./././././.dir");
	{
		char *input = "/../../../..////////hoge/./././././.dir";
		char *expected = "/hoge/.dir";
		char *actual = canonicalize_path(input);
		CHECK_EQ_STR(actual, expected);
		free(actual);
	}

	TEST_SECTION("canonicalize_path() /..///////hoge/////fuga/././././../../hoge2/./././/////");
	{
		char *input = "/..///////hoge/////fuga/././././../../hoge2/./././/////";
		char *expected = "/hoge2";
		char *actual = canonicalize_path(input);
		CHECK_EQ_STR(actual, expected);
		free(actual);
	}

	TEST_SECTION("change_to_parent_dir() /");
	{
		char *input = "/";
		char *expected = "/";
		char *actual = change_to_parent_dir(input);
		CHECK_EQ_STR(actual, expected);
		free(actual);
	}

	TEST_SECTION("change_to_parent_dir() /hoge");
	{
		char *input = "/hoge";
		char *expected = "/";
		char *actual = change_to_parent_dir(input);
		CHECK_EQ_STR(actual, expected);
		free(actual);
	}

	TEST_SECTION("change_to_parent_dir() /hoge/fuga");
	{
		char *input = "/hoge/fuga";
		char *expected = "/hoge";
		char *actual = change_to_parent_dir(input);
		CHECK_EQ_STR(actual, expected);
		free(actual);
	}

	int fail_count = print_result();
	return (fail_count);
}
