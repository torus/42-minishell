#include "test.h"
#include "../execution.h"
#include "../minishell.h"

t_shell	g_shell;

t_cmd_redirection *create_new_redir(char *filepath, int fd, bool is_append, bool is_heredoc)
{
	t_cmd_redirection	*new_redir;

	new_redir = calloc(1, sizeof(t_cmd_redirection));
	new_redir->filepath = ft_strdup(filepath);
	new_redir->fd = fd;
	new_redir->is_append = is_append;
	new_redir->is_heredoc = is_heredoc;
	new_redir->next= NULL;
	return (new_redir);
}

int check_redirs(t_cmd_redirection *redirs_actual, t_cmd_redirection *redirs_expected)
{
	while (redirs_actual && redirs_expected)
	{
		CHECK_EQ(redirs_actual->fd, redirs_expected->fd);
		CHECK_EQ_STR(redirs_actual->filepath, redirs_expected->filepath);
		CHECK_EQ(redirs_actual->is_append, redirs_actual->is_append);
		CHECK_EQ(redirs_actual->is_heredoc, redirs_actual->is_heredoc);
		redirs_actual = redirs_actual->next;
		redirs_expected = redirs_expected->next;
	}
	CHECK_EQ(redirs_actual, redirs_expected);
	if (redirs_actual != redirs_expected)
		return (1);
	return (0);
}

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

	TEST_SECTION("find_executable_file_in_dir() 実行可能ファイルが見つからない時");
	{
		char *fullpath = find_executable_file_in_dir("cat", "/");
		CHECK_EQ(fullpath, NULL);
		free(fullpath);
	}

	// cat < normal << heredoc < normal2
	TEST_SECTION("reds2in_fd_reds_list() 全て同じfdで最後は入力リダイレクション");
	{
		t_cmd_redirection *input_redirections = NULL;

		cmd_redirection_add_back(&input_redirections, create_new_redir("normal", 1, 0, 0));
		cmd_redirection_add_back(&input_redirections, create_new_redir("heredoc", 1, 0, 1));
		cmd_redirection_add_back(&input_redirections, create_new_redir("normal2", 1, 0, 0));

		t_fd_reds_list *fd_red_list_actual = cmd_reds2fd_reds_list(input_redirections);
		t_fd_reds_list *tmp = fd_red_list_actual;
		CHECK(fd_red_list_actual);
		CHECK_EQ(fd_red_list_actual->fd, 1);
		CHECK(fd_red_list_actual->reds);
		CHECK(fd_red_list_actual->heredoc_pipe[0] == -1);
		CHECK(fd_red_list_actual->heredoc_pipe[1] == -1);

		t_cmd_redirection *reds_expected = NULL;
		cmd_redirection_add_back(&reds_expected, create_new_redir("normal", 1, 0, 0));
		cmd_redirection_add_back(&reds_expected, create_new_redir("heredoc", 1, 0, 1));
		cmd_redirection_add_back(&reds_expected, create_new_redir("normal2", 1, 0, 0));
		check_redirs(fd_red_list_actual->reds, reds_expected);
		cmd_free_redirections(reds_expected);

		fd_red_list_actual = fd_red_list_actual->next;
		CHECK(!fd_red_list_actual);

		cmd_free_fd_reds_list(tmp);
		cmd_free_redirections(input_redirections);
	}

	// cat < normal << heredoc < normal2 << heredoc2
	TEST_SECTION("reds2in_fd_reds_list() 全て同じfdで最後はheredoc");
	{
		t_cmd_redirection *input_redirections = NULL;

		cmd_redirection_add_back(&input_redirections, create_new_redir("normal", 1, 0, 0));
		cmd_redirection_add_back(&input_redirections, create_new_redir("heredoc", 1, 0, 1));
		cmd_redirection_add_back(&input_redirections, create_new_redir("normal2", 1, 0, 0));
		cmd_redirection_add_back(&input_redirections, create_new_redir("heredoc2", 1, 0, 1));

		t_fd_reds_list *fd_red_list_actual = cmd_reds2fd_reds_list(input_redirections);
		t_fd_reds_list *tmp = fd_red_list_actual;
		CHECK(fd_red_list_actual);
		CHECK_EQ(fd_red_list_actual->fd, 1);
		CHECK(fd_red_list_actual->reds);

		t_cmd_redirection *reds_expected = NULL;
		cmd_redirection_add_back(&reds_expected, create_new_redir("normal", 1, 0, 0));
		cmd_redirection_add_back(&reds_expected, create_new_redir("heredoc", 1, 0, 1));
		cmd_redirection_add_back(&reds_expected, create_new_redir("normal2", 1, 0, 0));
		cmd_redirection_add_back(&reds_expected, create_new_redir("heredoc2", 1, 0, 1));
		check_redirs(fd_red_list_actual->reds, reds_expected);
		cmd_free_redirections(reds_expected);

		fd_red_list_actual = fd_red_list_actual->next;
		CHECK(!fd_red_list_actual);

		cmd_free_fd_reds_list(tmp);
		cmd_free_redirections(input_redirections);
	}

	// cat 1< normal 2<< heredoc 2< normal2 1<< heredoc2
	TEST_SECTION("reds2in_fd_reds_list() fdが異なる");
	{
		t_cmd_redirection *input_redirections = NULL;

		cmd_redirection_add_back(&input_redirections, create_new_redir("normal", 1, 0, 0));
		cmd_redirection_add_back(&input_redirections, create_new_redir("heredoc", 2, 0, 1));
		cmd_redirection_add_back(&input_redirections, create_new_redir("normal2", 2, 0, 0));
		cmd_redirection_add_back(&input_redirections, create_new_redir("heredoc2", 1, 0, 1));

		t_fd_reds_list *fd_red_list_actual = cmd_reds2fd_reds_list(input_redirections);
		t_fd_reds_list *tmp = fd_red_list_actual;
		CHECK(fd_red_list_actual);
		CHECK_EQ(fd_red_list_actual->fd, 1);
		CHECK(fd_red_list_actual->reds);

		t_cmd_redirection *reds_expected = NULL;
		cmd_redirection_add_back(&reds_expected, create_new_redir("normal", 1, 0, 0));
		cmd_redirection_add_back(&reds_expected, create_new_redir("heredoc2", 1, 0, 1));
		check_redirs(fd_red_list_actual->reds, reds_expected);
		cmd_free_redirections(reds_expected);

		fd_red_list_actual = fd_red_list_actual->next;
		CHECK(fd_red_list_actual);
		CHECK_EQ(fd_red_list_actual->fd, 2);
		CHECK(fd_red_list_actual->reds);

		reds_expected = NULL;
		cmd_redirection_add_back(&reds_expected, create_new_redir("heredoc", 2, 0, 1));
		cmd_redirection_add_back(&reds_expected, create_new_redir("normal2", 2, 0, 0));
		check_redirs(fd_red_list_actual->reds, reds_expected);
		cmd_free_redirections(reds_expected);

		fd_red_list_actual = fd_red_list_actual->next;
		CHECK(!fd_red_list_actual);

		cmd_free_fd_reds_list(tmp);
		cmd_free_redirections(input_redirections);
	}

	free_vars(g_shell.vars);
	int fail_count = print_result();
	return (fail_count);
}
