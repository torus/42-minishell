#include "test.h"
#include "../execution.h"

int main(){

    TEST_SECTION("echo hello");
    {
		t_command_invocation *command = cmd_init_cmdinvo((const char**)ft_split("echo hello", ' '));

		int status = cmd_exec_commands(command);
		CHECK_EQ(status, 0);
		cmd_free_cmdinvo(command);
    }

    TEST_SECTION("cat /etc/passwd > output.txt");
    {
		t_command_invocation *command = cmd_init_cmdinvo((const char**)ft_split("cat /etc/passwd", ' '));
		cmd_add_outredirect(command, ft_strdup("output.txt"), false);

		int status = cmd_exec_commands(command);
		CHECK_EQ(status, 0);
		CHECK(system("diff --color -u /etc/passwd output.txt") == 0);
		cmd_free_cmdinvo(command);
		remove("output.txt");
    }

	TEST_SECTION("リダイレクト先が既に存在しているファイルの場合");
    {
		system("echo aaaaaaaaaaaaaaaaaaaaaaaaaaaaa > output.txt ; cp output.txt expected.txt ; echo hello > expected.txt");
		t_command_invocation *command = cmd_init_cmdinvo((const char**)ft_split("echo hello", ' '));
		cmd_add_outredirect(command, ft_strdup("output.txt"), false);

		int status = cmd_exec_commands(command);
		CHECK_EQ(status, 0);
		CHECK(system("diff --color -u expected.txt output.txt") == 0);
		cmd_free_cmdinvo(command);
		remove("output.txt");
    }

    TEST_SECTION("/usr/bin/cat /etc/passwd > output.txt");
    {
		t_command_invocation *command = cmd_init_cmdinvo((const char**)ft_split("/usr/bin/cat /etc/passwd", ' '));
		cmd_add_outredirect(command, ft_strdup("output.txt"), false);

		int status = cmd_exec_commands(command);
		CHECK_EQ(status, 0);
		CHECK(system("diff --color -u /etc/passwd output.txt") == 0);
		cmd_free_cmdinvo(command);
		remove("output.txt");
    }

    TEST_SECTION("echo abc > actual.txt ; echo def >> actual.txt  追記リダイレクト");
    {
		system("echo abc > actual.txt");
		t_command_invocation *command = cmd_init_cmdinvo((const char**)ft_split("echo def", ' '));
		cmd_add_outredirect(command, ft_strdup("actual.txt"), true);

		int status = cmd_exec_commands(command);
		CHECK_EQ(status, 0);
		CHECK(system("echo abc > expected.txt ; echo def >> expected.txt" " && "
				"diff --color -u expected.txt actual.txt") == 0);
		cmd_free_cmdinvo(command);
		remove("actual.txt");
		remove("expected.txt");
    }

    TEST_SECTION("cat < Makefile");
    {
		t_command_invocation *command = cmd_init_cmdinvo((const char**)ft_split("cat", ' '));
		cmd_add_inredirect(command, ft_strdup("Makefile"));

		int status = cmd_exec_commands(command);
		CHECK_EQ(status, 0);
		cmd_free_cmdinvo(command);
    }

    TEST_SECTION("/usr/bin/cat < Makefile");
    {
		t_command_invocation *command = cmd_init_cmdinvo((const char**)ft_split("/usr/bin/cat", ' '));
		cmd_add_inredirect(command, ft_strdup("Makefile"));

		int status = cmd_exec_commands(command);
		CHECK_EQ(status, 0);
		cmd_free_cmdinvo(command);
    }

    TEST_SECTION("cat < not_exists  存在しないファイルを入力リダイレクトするとエラー");
    {
		t_command_invocation *command = cmd_init_cmdinvo((const char**)ft_split("cat", ' '));
		cmd_add_inredirect(command, ft_strdup("not_exists"));

		int status = cmd_exec_commands(command);
		CHECK(status != 0);
		cmd_free_cmdinvo(command);
    }

    TEST_SECTION("cat < test.h > output.txt");
    {
		t_command_invocation *command = cmd_init_cmdinvo((const char**)ft_split("cat", ' '));
		cmd_add_inredirect(command, ft_strdup("test.h"));
		cmd_add_outredirect(command, ft_strdup("output.txt"), false);

		int status = cmd_exec_commands(command);
		CHECK_EQ(status, 0);
		CHECK(system("diff --color -u test.h output.txt") == 0);
		cmd_free_cmdinvo(command);
		remove("output.txt");
    }

    TEST_SECTION("cat Makefile > a > b  出力リダイレクトが複数");
    {
		t_command_invocation *command = cmd_init_cmdinvo((const char**)ft_split("cat Makefile", ' '));
		cmd_add_outredirect(command, ft_strdup("actual_a"), false);
		cmd_add_outredirect(command, ft_strdup("actual_b"), false);

		int status = cmd_exec_commands(command);
		CHECK_EQ(status, 0);
		CHECK(system("cat Makefile > expected_a > expected_b" " && "
			  "diff --color -u expected_a actual_a && diff --color -u expected_b actual_b") == 0);
		cmd_free_cmdinvo(command);
		remove("actual_a");
		remove("actual_b");
		remove("expected_a");
		remove("expected_b");
    }

    TEST_SECTION("cat < Makefile < test.h > output.txt  入力リダイレクトが複数");
    {
		t_command_invocation *command = cmd_init_cmdinvo((const char**)ft_split("cat", ' '));
		cmd_add_inredirect(command, ft_strdup("Makefile"));
		cmd_add_inredirect(command, ft_strdup("test.h"));
		cmd_add_outredirect(command, ft_strdup("output.txt"), false);

		int status = cmd_exec_commands(command);
		CHECK_EQ(status, 0);
		CHECK(system("cat < Makefile < test.h > expected.txt" " && "
			  "diff --color -u expected.txt output.txt") == 0);
		cmd_free_cmdinvo(command);
		remove("expected.txt");
		remove("output.txt");
    }

	TEST_SECTION("cat Makefile >> a >> b 追記リダイレクションの時は途中のファイルは上書きされない");
    {
		system("echo ThisIsA > actual_a ; echo ThisIsB > actual_b");
		t_command_invocation *command = cmd_init_cmdinvo((const char**)ft_split("cat Makefile", ' '));
		cmd_add_outredirect(command, ft_strdup("actual_a"), true);
		cmd_add_outredirect(command, ft_strdup("actual_b"), true);

		int status = cmd_exec_commands(command);
		CHECK_EQ(status, 0);
		system("echo ThisIsA > expected_a ; echo ThisIsB > expected_b");
		CHECK(system("cat Makefile >> expected_a >> expected_b" " && "
			  "diff --color -u expected_a actual_a && diff --color -u expected_b actual_b") == 0);
		cmd_free_cmdinvo(command);
		remove("actual_a");
		remove("actual_b");
		remove("expected_a");
		remove("expected_b");
    }

    TEST_SECTION("cat /etc/passwd | wc > output.txt  パイプが1つの場合");
    {
		t_command_invocation *first_command = cmd_init_cmdinvo((const char**)ft_split("cat /etc/passwd", ' '));

		t_command_invocation *second_command = cmd_init_cmdinvo((const char**)ft_split("wc", ' '));
		cmd_add_outredirect(second_command, ft_strdup("output.txt"), false);

		cmd_add_cmdinvo(&first_command, second_command);

		int status = cmd_exec_commands(first_command);
		CHECK_EQ(status, 0);
		CHECK(system("cat /etc/passwd | wc > expected.txt ; diff --color -u expected.txt output.txt") == 0);
		cmd_free_cmdinvo(first_command);
		remove("output.txt");
    }

    TEST_SECTION("cat /etc/passwd | wc | cat > output.txt  パイプが2つの場合");
    {
		t_command_invocation *first_command = cmd_init_cmdinvo((const char**)ft_split("cat /etc/passwd", ' '));

		t_command_invocation *second_command = cmd_init_cmdinvo((const char**)ft_split("wc", ' '));

		t_command_invocation *third_command = cmd_init_cmdinvo((const char**)ft_split("cat", ' '));
		cmd_add_outredirect(third_command, ft_strdup("output.txt"), false);

		cmd_add_cmdinvo(&first_command, second_command);
		cmd_add_cmdinvo(&first_command, third_command);

		int status = cmd_exec_commands(first_command);
		CHECK_EQ(status, 0);
		CHECK(system("cat /etc/passwd | wc | cat > expected.txt ; diff --color -u expected.txt output.txt") == 0);
		cmd_free_cmdinvo(first_command);
		remove("output.txt");
    }

    TEST_SECTION("cat /etc/passwd > /dev/null | wc | cat > output.txt  パイプの最初のコマンドでリダイレクトがある");
    {
		t_command_invocation *first_command = cmd_init_cmdinvo((const char**)ft_split("cat /etc/passwd", ' '));
		cmd_add_outredirect(first_command, ft_strdup("/dev/null"), false);

		t_command_invocation *second_command = cmd_init_cmdinvo((const char**)ft_split("wc", ' '));

		t_command_invocation *third_command = cmd_init_cmdinvo((const char**)ft_split("cat", ' '));
		cmd_add_outredirect(third_command, ft_strdup("output.txt"), false);

		cmd_add_cmdinvo(&first_command, second_command);
		cmd_add_cmdinvo(&first_command, third_command);

		int status = cmd_exec_commands(first_command);
		CHECK_EQ(status, 0);
		CHECK(system("cat /etc/passwd > /dev/null | wc | cat > expected.txt ; diff --color -u expected.txt output.txt") == 0);
		cmd_free_cmdinvo(first_command);
		remove("output.txt");
    }

    TEST_SECTION("cat test.h | wc < test.c | cat > output.txt  パイプの真ん中のコマンドでリダイレクトがある");
    {
		t_command_invocation *first_command = cmd_init_cmdinvo((const char**)ft_split("cat test.h", ' '));

		t_command_invocation *second_command = cmd_init_cmdinvo((const char**)ft_split("wc", ' '));
		cmd_add_inredirect(second_command, ft_strdup("test.c"));

		t_command_invocation *third_command = cmd_init_cmdinvo((const char**)ft_split("cat", ' '));
		cmd_add_outredirect(third_command, ft_strdup("output.txt"), false);

		cmd_add_cmdinvo(&first_command, second_command);
		cmd_add_cmdinvo(&first_command, third_command);

		int status = cmd_exec_commands(first_command);
		CHECK_EQ(status, 0);
		CHECK(system("cat test.h | wc < test.c | cat > expected.txt ; diff --color -u expected.txt output.txt") == 0);
		cmd_free_cmdinvo(first_command);
		remove("output.txt");
    }

    TEST_SECTION("存在するコマンドを実行すると返り値は0になる");
    {
		t_command_invocation *command = cmd_init_cmdinvo((const char**)ft_split("echo", ' '));

		int status = cmd_exec_commands(command);
		CHECK(status == 0);
		cmd_free_cmdinvo(command);
    }

    TEST_SECTION("存在しないコマンドを実行すると返り値は0以外になる");
    {
		t_command_invocation *command = cmd_init_cmdinvo((const char**)ft_split("not_exists", ' '));

		int status = cmd_exec_commands(command);
		CHECK(status != 0);
		cmd_free_cmdinvo(command);
    }

    TEST_SECTION("存在しないコマンドを実行してもリダイレクト先のファイルは作成される");
    {
		t_command_invocation *command = cmd_init_cmdinvo((const char**)ft_split("not_exists", ' '));
		cmd_add_outredirect(command, ft_strdup("output.txt"), false);

		remove("output.txt");
		int status = cmd_exec_commands(command);
		CHECK(status != 0);
		CHECK(system("ls -l output.txt") == 0);
		cmd_free_cmdinvo(command);
		remove("output.txt");
    }

	int fail_count = print_result();
	return (fail_count);
}
