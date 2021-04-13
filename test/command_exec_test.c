#include "test.h"
#include "../execution.h"

int main(){

    TEST_SECTION("echo hello");
    {
		t_command_invocation command;
		command.output_file_path = NULL;
		command.piped_command = NULL;
		command.input_file_path = NULL;
		command.exec_and_args = (const char**)ft_split("echo hello", ' ');

		cmd_command_execution(&command);
		free_ptrarr((void**)command.exec_and_args);
    }

    TEST_SECTION("cat /etc/passwd > output.txt");
    {
		t_command_invocation command;
		command.output_file_path = "output.txt";
		command.piped_command = NULL;
		command.input_file_path = NULL;
		command.exec_and_args = (const char**)ft_split("cat /etc/passwd", ' ');

		cmd_command_execution(&command);
		free_ptrarr((void**)command.exec_and_args);
		CHECK(system("diff --color /etc/passwd output.txt") == 0);
		remove("output.txt");
    }

    TEST_SECTION("/usr/bin/cat /etc/passwd > output.txt");
    {
		t_command_invocation command;
		command.output_file_path = "output.txt";
		command.piped_command = NULL;
		command.input_file_path = NULL;
		command.exec_and_args = (const char**)ft_split("/usr/bin/cat /etc/passwd", ' ');

		cmd_command_execution(&command);
		free_ptrarr((void**)command.exec_and_args);
		CHECK(system("diff --color /etc/passwd output.txt") == 0);
		remove("output.txt");
    }

    TEST_SECTION("cat < Makefile");
    {
		t_command_invocation command;
		command.output_file_path = NULL;
		command.piped_command = NULL;
		command.input_file_path = "Makefile";
		command.exec_and_args = (const char**)ft_split("cat", ' ');

		cmd_command_execution(&command);
		free_ptrarr((void**)command.exec_and_args);
    }

    TEST_SECTION("/usr/bin/cat < Makefile");
    {
		t_command_invocation command;
		command.output_file_path = NULL;
		command.piped_command = NULL;
		command.input_file_path = "Makefile";
		command.exec_and_args = (const char**)ft_split("/usr/bin/cat", ' ');

		cmd_command_execution(&command);
		free_ptrarr((void**)command.exec_and_args);
    }

    TEST_SECTION("cat < test.h > output.txt");
    {
		t_command_invocation command;
		command.output_file_path = "output.txt";
		command.piped_command = NULL;
		command.input_file_path = "test.h";
		command.exec_and_args = (const char**)ft_split("cat", ' ');

		cmd_command_execution(&command);
		free_ptrarr((void**)command.exec_and_args);
		CHECK(system("diff --color test.h output.txt") == 0);
		remove("output.txt");
    }

    TEST_SECTION("cat /etc/passwd | wc > output.txt  パイプが1つの場合");
    {
		t_command_invocation wc_command;
		wc_command.output_file_path = "output.txt";
		wc_command.piped_command = NULL;
		wc_command.input_file_path = NULL;
		wc_command.exec_and_args = (const char**)ft_split("wc", ' ');

		t_command_invocation cat_command;
		cat_command.output_file_path = NULL;
		cat_command.piped_command = &wc_command;
		cat_command.input_file_path = NULL;
		cat_command.exec_and_args = (const char**)ft_split("cat /etc/passwd", ' ');

		cmd_command_execution(&cat_command);
		free_ptrarr((void**)cat_command.exec_and_args);
		free_ptrarr((void**)wc_command.exec_and_args);
		CHECK(system("cat /etc/passwd | wc > expected.txt ; diff --color expected.txt output.txt") == 0);
		remove("output.txt");
    }

    TEST_SECTION("cat /etc/passwd | wc | md5sum > output.txt  パイプが2つの場合");
    {
		t_command_invocation md5sum_command;
		md5sum_command.output_file_path = "output.txt";
		md5sum_command.piped_command = NULL;
		md5sum_command.input_file_path = NULL;
		md5sum_command.exec_and_args = (const char**)ft_split("md5sum", ' ');

		t_command_invocation wc_command;
		wc_command.output_file_path = NULL;
		wc_command.piped_command = &md5sum_command;
		wc_command.input_file_path = NULL;
		wc_command.exec_and_args = (const char**)ft_split("wc", ' ');

		t_command_invocation cat_command;
		cat_command.output_file_path = NULL;
		cat_command.piped_command = &wc_command;
		cat_command.input_file_path = NULL;
		cat_command.exec_and_args = (const char**)ft_split("cat /etc/passwd", ' ');

		cmd_command_execution(&cat_command);
		free_ptrarr((void**)cat_command.exec_and_args);
		free_ptrarr((void**)wc_command.exec_and_args);
		CHECK(system("cat /etc/passwd | wc | md5sum > expected.txt ; diff --color expected.txt output.txt") == 0);
		remove("output.txt");
    }

    TEST_SECTION("cat /etc/passwd > /dev/null | wc | md5sum > output.txt  パイプの最初のコマンドでリダイレクトがある");
    {
		t_command_invocation md5sum_command;
		md5sum_command.output_file_path = "output.txt";
		md5sum_command.piped_command = NULL;
		md5sum_command.input_file_path = NULL;
		md5sum_command.exec_and_args = (const char**)ft_split("md5sum", ' ');

		t_command_invocation wc_command;
		wc_command.output_file_path = NULL;
		wc_command.piped_command = &md5sum_command;
		wc_command.input_file_path = NULL;
		wc_command.exec_and_args = (const char**)ft_split("wc", ' ');

		t_command_invocation cat_command;
		cat_command.output_file_path = "/dev/null";
		cat_command.piped_command = &wc_command;
		cat_command.input_file_path = NULL;
		cat_command.exec_and_args = (const char**)ft_split("cat /etc/passwd", ' ');

		cmd_command_execution(&cat_command);
		free_ptrarr((void**)cat_command.exec_and_args);
		free_ptrarr((void**)wc_command.exec_and_args);
		CHECK(system("cat /etc/passwd > /dev/null | wc | md5sum > expected.txt ; diff --color expected.txt output.txt") == 0);
		remove("output.txt");
    }

    TEST_SECTION("cat test.h | wc < test.c | md5sum > output.txt  パイプの真ん中のコマンドでリダイレクトがある");
    {
		t_command_invocation md5sum_command;
		md5sum_command.output_file_path = "output.txt";
		md5sum_command.piped_command = NULL;
		md5sum_command.input_file_path = NULL;
		md5sum_command.exec_and_args = (const char**)ft_split("md5sum", ' ');

		t_command_invocation wc_command;
		wc_command.output_file_path = NULL;
		wc_command.piped_command = &md5sum_command;
		wc_command.input_file_path = "test.c";
		wc_command.exec_and_args = (const char**)ft_split("wc", ' ');

		t_command_invocation cat_command;
		cat_command.output_file_path = NULL;
		cat_command.piped_command = &wc_command;
		cat_command.input_file_path = NULL;
		cat_command.exec_and_args = (const char**)ft_split("cat test.h", ' ');

		cmd_command_execution(&cat_command);
		free_ptrarr((void**)cat_command.exec_and_args);
		free_ptrarr((void**)wc_command.exec_and_args);
		CHECK(system("cat test.h | wc < test.c | md5sum > expected.txt ; diff --color expected.txt output.txt") == 0);
		remove("output.txt");
    }

	int fail_count = print_result();
	return (fail_count);
}
