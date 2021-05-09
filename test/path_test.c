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

	TEST_SECTION("canonicalize_path() ///////usr/////bin");
	{
		char *input = "///////usr/////bin";
		char *expected = "/usr/bin";
		char *actual = canonicalize_path(input);
		CHECK_EQ_STR(actual, expected);
		free(actual);
	}

	TEST_SECTION("canonicalize_path() /../../../..////////usr/./././././bin");
	{
		char *input = "/../../../..////////usr/./././././bin";
		char *expected = "/usr/bin";
		char *actual = canonicalize_path(input);
		CHECK_EQ_STR(actual, expected);
		free(actual);
	}

	TEST_SECTION("canonicalize_path() /..///////usr/////bin/././././../../usr/./././/////");
	{
		char *input = "/..///////usr/////bin/././././../../bin/./././/////";
		char *expected = "/bin";
		char *actual = canonicalize_path(input);
		CHECK_EQ_STR(actual, expected);
		free(actual);
	}

	/*
	 * 歴史的な理由から先頭のスラッシュが2つの場合はそのままになる.
	 * これは bash の cd の仕様に合わせたものである.
	 * http://tiswww.case.edu/php/chet/bash/FAQ の E10 を参照
	 */
	TEST_SECTION("canonicalize_path() //usr//bin");
	{
		char *input = "//usr//bin";
		char *expected = "//usr/bin";
		char *actual = canonicalize_path(input);
		CHECK_EQ_STR(actual, expected);
		free(actual);
	}

	/* 途中で存在しないディレクトリが入ってたらNULLが返ってくる */
	TEST_SECTION("canonicalize_path() //usr//hoge/./././/../../usr");
	{
		char *input = "//usr//hoge/./././/../../usr";
		char *expected = NULL;
		char *actual = canonicalize_path(input);
		CHECK_EQ(actual, expected);
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
