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

		command_execution(&command);
		free_ptrarr((void**)command.exec_and_args);
    }

    TEST_SECTION("cat /etc/passwd > output.txt");
    {
		t_command_invocation command;
		command.output_file_path = "output.txt";
		command.piped_command = NULL;
		command.input_file_path = NULL;
		command.exec_and_args = (const char**)ft_split("cat /etc/passwd", ' ');

		command_execution(&command);
		free_ptrarr((void**)command.exec_and_args);
		CHECK(system("diff /etc/passwd output.txt") == 0);
		remove("output.txt");
    }

    TEST_SECTION("/usr/bin/cat /etc/passwd > output.txt");
    {
		t_command_invocation command;
		command.output_file_path = "output.txt";
		command.piped_command = NULL;
		command.input_file_path = NULL;
		command.exec_and_args = (const char**)ft_split("/usr/bin/cat /etc/passwd", ' ');

		command_execution(&command);
		free_ptrarr((void**)command.exec_and_args);
		CHECK(system("diff /etc/passwd output.txt") == 0);
		remove("output.txt");
    }

    TEST_SECTION("cat < Makefile");
    {
		t_command_invocation command;
		command.output_file_path = NULL;
		command.piped_command = NULL;
		command.input_file_path = "Makefile";
		command.exec_and_args = (const char**)ft_split("cat", ' ');

		command_execution(&command);
		free_ptrarr((void**)command.exec_and_args);
    }

    TEST_SECTION("/usr/bin/cat < Makefile");
    {
		t_command_invocation command;
		command.output_file_path = NULL;
		command.piped_command = NULL;
		command.input_file_path = "Makefile";
		command.exec_and_args = (const char**)ft_split("/usr/bin/cat", ' ');

		command_execution(&command);
		free_ptrarr((void**)command.exec_and_args);
    }

    TEST_SECTION("cat < test.h > output.txt");
    {
		t_command_invocation command;
		command.output_file_path = "output.txt";
		command.piped_command = NULL;
		command.input_file_path = "test.h";
		command.exec_and_args = (const char**)ft_split("cat", ' ');

		command_execution(&command);
		free_ptrarr((void**)command.exec_and_args);
		CHECK(system("diff test.h output.txt") == 0);
		remove("output.txt");
    }

	/*
    TEST_SECTION("cat /etc/passwd | wc > output.txt");
    {
		t_command_invocation wc_command;
		wc_command.output_file_path = "output.txt";
		wc_command.piped_command = NULL;
		wc_command.input_file_path = NULL;
		wc_command.exec_and_args = (const char**)ft_split("wc", ' ');

		t_command_invocation cat_command;
		cat_command.output_file_path = NULL;
		cat_command.piped_command = &wc_command;
		cat_command.input_file_path = "/etc/passwd";
		cat_command.exec_and_args = (const char**)ft_split("cat /etc/passwd", ' ');

		command_execution(&cat_command);
		free_ptrarr((void**)cat_command.exec_and_args);
		free_ptrarr((void**)wc_command.exec_and_args);
		CHECK(system("diff <(cat /etc/passwd | wc) output.txt") == 0);
		remove("output.txt");
    }
	*/

	int fail_count = print_result();
	return (fail_count);
}
