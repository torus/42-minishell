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
		free_ptrarr((void**)command.exec_and_args);
    }

    TEST_SECTION("cat /etc/passwd > passwd_tmp.txt");
    {
		t_command_invocation command;
		command.output_file_path = "passwd_tmp.txt";
		command.piped_command = NULL;
		command.input_file_path = NULL;
		command.exec_and_args = (const char**)ft_split("cat /etc/passwd", ' ');
		free_ptrarr((void**)command.exec_and_args);
    }

    TEST_SECTION("cat < /etc/passwd");
    {
		t_command_invocation command;
		command.output_file_path = NULL;
		command.piped_command = NULL;
		command.input_file_path = "/etc/passwd";
		command.exec_and_args = (const char**)ft_split("cat", ' ');
		free_ptrarr((void**)command.exec_and_args);
    }

    TEST_SECTION("cat /etc/passwd | wc > wc_tmp.txt");
    {
		t_command_invocation wc_command;
		wc_command.output_file_path = "wc_tmp.txt";
		wc_command.piped_command = NULL;
		wc_command.input_file_path = NULL;
		wc_command.exec_and_args = (const char**)ft_split("wc", ' ');

		t_command_invocation cat_command;
		cat_command.output_file_path = NULL;
		cat_command.piped_command = &wc_command;
		cat_command.input_file_path = "/etc/passwd";
		cat_command.exec_and_args = (const char**)ft_split("cat /etc/passwd", ' ');

		free_ptrarr((void**)cat_command.exec_and_args);
		free_ptrarr((void**)wc_command.exec_and_args);
    }
}
