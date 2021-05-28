#include "test.h"
#include "../env.h"
#include "../libft/libft.h"
#include "../minishell.h"

t_shell	g_shell;

void check_strarr(const char **actual_strarr, const char **expected_strarr)
{
	int i = 0 ;
	while (actual_strarr[i] && expected_strarr[i])
	{
		printf("%d:\n", i);
		CHECK_EQ_STR(actual_strarr[i], expected_strarr[i]);
		i++;
	}
	printf("%d: |%s| == |%s|\n", i, actual_strarr[i], expected_strarr[i]);
	CHECK(!(actual_strarr[i] || expected_strarr[i]));  // 両方ともNULLだよね?
}

int main(){
	init_g_shell();

	TEST_SECTION("split_first_c() 普通に\"Key=Value\"");
	{
		const char *input = "PATH=/bin/:/usr/bin/:/home/jun/bin";
		char **str_arr = split_first_c(input, '=');
		CHECK_EQ_STR(str_arr[0], "PATH");
		CHECK_EQ_STR(str_arr[1], "/bin/:/usr/bin/:/home/jun/bin");
		CHECK_EQ(str_arr[2], NULL);

		free_ptrarr((void **)str_arr);
	}

	TEST_SECTION("split_first_c() 区切り文字が入っていない");
	{
		const char *input = "PATH/bin/:/usr/bin/:/home/jun/bin";
		char **str_arr = split_first_c(input, '=');
		CHECK_EQ_STR(str_arr[0], "PATH/bin/:/usr/bin/:/home/jun/bin");
		CHECK_EQ(str_arr[1], NULL);
		CHECK_EQ(str_arr[2], NULL);

		free_ptrarr((void **)str_arr);
	}

	TEST_SECTION("split_first_c() 最後に区切り文字\"key=\"");
	{
		const char *input = "PATH=";
		char **str_arr = split_first_c(input, '=');
		CHECK_EQ_STR(str_arr[0], "PATH");
		CHECK_EQ_STR(str_arr[1], "");
		CHECK_EQ(str_arr[2], NULL);

		free_ptrarr((void **)str_arr);
	}

	TEST_SECTION("get_colon_units() \":/hoge/::/\"");
	{
		const char *input = ":/hoge/::/";
		const char *default_str = "";
		char **str_arr = get_colon_units(input, default_str);
		CHECK_EQ_STR(str_arr[0], "");
		CHECK_EQ_STR(str_arr[1], "/hoge/");
		CHECK_EQ_STR(str_arr[2], "");
		CHECK_EQ_STR(str_arr[3], "/");
		CHECK_EQ(str_arr[4], NULL);

		free_ptrarr((void **)str_arr);
	}

	TEST_SECTION("get_colon_units() \"/hoge/::/::\"");
	{
		const char *input = "/hoge/::/::";
		const char *default_str = "./";
		char **str_arr = get_colon_units(input, default_str);
		CHECK_EQ_STR(str_arr[0], "/hoge/");
		CHECK_EQ_STR(str_arr[1], "./");
		CHECK_EQ_STR(str_arr[2], "/");
		CHECK_EQ_STR(str_arr[3], "./");
		CHECK_EQ_STR(str_arr[4], "./");
		CHECK_EQ(str_arr[5], NULL);

		free_ptrarr((void **)str_arr);
	}

	TEST_SECTION("get_env() 通常ケース");
	{
		add_new_var(&g_shell.vars, "GET_ENV_TEST", "/bin/:/usr/bin/:/home/jun/bin", 0);
		t_var *actual = get_env("GET_ENV_TEST");
		CHECK_EQ_STR(actual->key, "GET_ENV_TEST");
		CHECK_EQ_STR(actual->value, "/bin/:/usr/bin/:/home/jun/bin");
		ft_unsetenv("GET_ENV_TEST");
	}

	TEST_SECTION("get_env() 指定した環境変数が存在しない場合");
	{
		ft_unsetenv("THIS_IS_NOT_EXIST_IN_ENVIRON");
		t_var *actual = get_env("THIS_IS_NOT_EXIST_IN_ENVIRON");
		CHECK_EQ(actual, NULL);
	}

	TEST_SECTION("get_env() 途中まで同じ環境変数名な時");
	{
		ft_unsetenv("GET_ENV_TEST");
		ft_setenv("GET_ENV_TEST_TEST", "/bin/:/usr/bin/:/home/jun/bin", 0);
		t_var *var = get_env("GET_ENV_TEST");
		CHECK_EQ(var, NULL);
		ft_unsetenv("GET_ENV_TEST_TEST");
	}

	TEST_SECTION("get_val_from_kvstr() 通常ケース");
	{
		const char *kvstr = "PATH=/bin/:/usr/bin/:/home/jun/bin";
		const char *val_str = get_val_from_kvstr(kvstr, '=');
		CHECK_EQ_STR(val_str, "/bin/:/usr/bin/:/home/jun/bin");

		free((void*)val_str);
	}

	TEST_SECTION("get_val_from_kvstr() 区切り文字が無い");
	{
		const char *kvstr = "PATH/bin/:/usr/bin/:/home/jun/bin";
		const char *val_str = get_val_from_kvstr(kvstr, '=');
		CHECK_EQ(val_str, NULL);

		free((void*)val_str);
	}

	TEST_SECTION("get_val_from_kvstr() 区切り文字が連続で並ぶ");
	{
		const char *kvstr = "PATH===/bin/:/usr/bin/:/home/jun/bin";
		const char *val_str = get_val_from_kvstr(kvstr, '=');
		CHECK_EQ_STR(val_str, "==/bin/:/usr/bin/:/home/jun/bin");

		free((void*)val_str);
	}

	TEST_SECTION("exit_status");
	{
		set_status(0);
		CHECK_EQ(get_status(), 0);
		set_status(1);
		CHECK_EQ(get_status(), 1);
		set_status(-1);
		CHECK_EQ(get_status(), -1);
	}

	TEST_CHAPTER("env_setter");
	/* env_setter関連のテストケースでは
	 * 標準ライブラリ関数である setenv() や unsetenv() などは
	 * 使っていません.
	 * なぜなら, 標準ライブラリ関数では内部でrealloc()などを使っているのですが,
	 * それが ft_setenv() と上手く噛み合わず, ダブルフリーすることになります.
	 * だからといって ft_setenv() 内でfreeしない場合はメモリリークになります.
	 * なのでテストケース内で 標準ライブラリ関数の setenv() や unsetenv() は使ってません.
	 */

	TEST_SECTION("ft_setenv() update rewrite=0");
	{
		ft_setenv("ABC", "ft_setenv() update rewrite=0", 1);
		t_var	*prev = get_env("ABC");
		CHECK(ft_setenv("ABC", "ft_setenv()", 0) == 0);
		t_var	*new = get_env("ABC");
		CHECK_EQ_STR(new->value, prev->value);
		ft_unsetenv("ABC");
	}

	TEST_SECTION("ft_setenv() expand_and_add_env");
	{
		ft_unsetenv("ABC");
		t_var	*prev = get_env("ABC");
		CHECK(!prev);
		CHECK(ft_setenv("ABC", "ft_setenv()", 0) == 0);
		t_var	*new = get_env("ABC");
		CHECK_EQ_STR(new->value, "ft_setenv()");
	}

	TEST_SECTION("ft_unsetenv() unset env");
	{
		ft_setenv("ABC", "ft_unsetenv() unset env", 1);
		t_var	*prev = get_env("ABC");
		CHECK(prev);
		CHECK(ft_unsetenv("ABC") == 0);
		t_var	*new = get_env("ABC");
		CHECK(!new);
		ft_unsetenv("ABC");
	}

	TEST_SECTION("ft_unsetenv() unset env that doesn't exist");
	{
		ft_unsetenv("ABC");
		t_var	*prev = get_env("ABC");
		CHECK(!prev);
		CHECK(ft_unsetenv("ABC") == 0);
		t_var	*new = get_env("ABC");
		CHECK(!new);
		ft_unsetenv("ABC");
	}

	TEST_CHAPTER("t_var");

	TEST_SECTION("kvstr2t_var");
	{
		char *kvstr = "env1=this is env1";
		t_var *var = kvstr2var(kvstr, 0);
		CHECK(var);
		CHECK_EQ_STR(var->key, "env1");
		CHECK_EQ_STR(var->value, "this is env1");
		CHECK_EQ(var->is_shell_var, 0);
		CHECK_EQ(var->next, NULL);
		free_vars(var);
	}

	TEST_SECTION("kvstr2t_var 空文字列");
	{
		char *kvstr = "env1=";
		t_var *var = kvstr2var(kvstr, 0);
		CHECK(var);
		CHECK_EQ_STR(var->key, "env1");
		CHECK_EQ_STR(var->value, "");
		CHECK_EQ(var->is_shell_var, 0);
		CHECK_EQ(var->next, NULL);
		free_vars(var);
	}

	TEST_SECTION("kvstr2t_var 値なし");
	{
		char *kvstr = "env1";
		t_var *var = kvstr2var(kvstr, 0);
		CHECK(var);
		CHECK_EQ_STR(var->key, "env1");
		CHECK_EQ(var->value, NULL);
		CHECK_EQ(var->is_shell_var, 0);
		CHECK_EQ(var->next, NULL);
		free_vars(var);
	}

	TEST_SECTION("文字列配列をt_varに変換する");
	{
		char	**envarr;
		t_var	*vars;

		envarr = ft_split("env1=this_is_env1 env2=this_is_env2 env3=this_is_env3", ' ');

		vars = environ2vars(envarr);
		t_var *tmp = vars;
		CHECK_EQ_STR(tmp->key, "env1");
		CHECK_EQ_STR(tmp->value, "this_is_env1");
		CHECK_EQ(tmp->is_shell_var, 0);
		tmp = tmp->next;
		CHECK_EQ_STR(tmp->key, "env2");
		CHECK_EQ_STR(tmp->value, "this_is_env2");
		CHECK_EQ(tmp->is_shell_var, 0);
		tmp = tmp->next;
		CHECK_EQ_STR(tmp->key, "env3");
		CHECK_EQ_STR(tmp->value, "this_is_env3");
		CHECK_EQ(tmp->is_shell_var, 0);
		free_vars(vars);
		free_ptrarr((void **)envarr);
	}

	TEST_SECTION("文字列配列をt_varに変換して変数を追加し, 文字列配列に戻す");
	{
		char	**envarr_expected;
		char	**envarr_actual;
		t_var	*vars;

		envarr_expected = ft_split("env1=env1 env2=env2 env3=env3", ' ');

		vars = environ2vars(envarr_expected);
		add_new_var(&vars, "NEW_ENV", "new env", 0);

		envarr_actual = vars2environ(vars);
		CHECK(envarr_actual);

		char	**tmp = envarr_expected;
		envarr_expected = (char **)ptrarr_add_ptr((void **)envarr_expected, ft_strdup("NEW_ENV=new env"));
		free(tmp);

		sort_strarr(envarr_actual);
		sort_strarr(envarr_expected);
		check_strarr((const char**)envarr_actual, (const char**)envarr_expected);
		free_ptrarr((void **)envarr_actual);
		free_ptrarr((void **)envarr_expected);
		free_vars(vars);
	}

	TEST_SECTION("vars2environ");
	{
		t_var	*vars;

		vars = NULL;
		add_new_var(&vars, "ENV_VAR1", "env1", 0);
		add_new_var(&vars, "ENV_VAR2", NULL, 0);  // これはenvironには含まれない
		add_new_var(&vars, "ENV_VAR3", "", 0);
		add_new_var(&vars, "SHELL_VAR1", "shell1", 1);
		add_new_var(&vars, "SHELL_VAR2", "shell2", 1);
		add_new_var(&vars, "SHELL_VAR3", "shell3", 1);

		char	**envarr_expected;
		char	**envarr_actual;
		envarr_expected = ft_split("ENV_VAR1=env1 ENV_VAR3=", ' ');
		envarr_actual = vars2environ(vars);
		check_strarr((const char**)envarr_actual, (const char**)envarr_expected);
		free_ptrarr((void **)envarr_actual);
		free_ptrarr((void **)envarr_expected);
		free_vars(vars);
	}

	TEST_SECTION("シェル変数");
	{
		t_var	*vars;

		vars = NULL;
		add_new_var(&vars, "ENV_VAR1", "env1", 0);
		add_new_var(&vars, "ENV_VAR2", "env2", 0);
		add_new_var(&vars, "ENV_VAR3", "env3", 0);
		add_new_var(&vars, "SHELL_VAR1", "shell1", 1);
		add_new_var(&vars, "SHELL_VAR2", "shell2", 1);
		add_new_var(&vars, "SHELL_VAR3", "shell3", 1);

		char	**envarr_expected;
		char	**envarr_actual;
		envarr_expected = ft_split("ENV_VAR1=env1 ENV_VAR2=env2 ENV_VAR3=env3", ' ');
		envarr_actual = vars2environ(vars);
		check_strarr((const char**)envarr_actual, (const char**)envarr_expected);
		free_ptrarr((void **)envarr_actual);
		free_ptrarr((void **)envarr_expected);
		free_vars(vars);
	}

	TEST_SECTION("シェル変数から環境変数にexport");
	{
		free_vars(g_shell.vars);
		g_shell.vars = NULL;

		ft_setenv("VAR", "shell_var", 1);
		CHECK_EQ_STR(g_shell.vars->key, "VAR");
		CHECK_EQ_STR(g_shell.vars->value, "shell_var");
		CHECK_EQ(g_shell.vars->is_shell_var, 1);

		ft_setenv("VAR", "env_var", 0);
		CHECK_EQ_STR(g_shell.vars->key, "VAR");
		CHECK_EQ_STR(g_shell.vars->value, "env_var");
		CHECK_EQ(g_shell.vars->is_shell_var, 0);
		CHECK_EQ(g_shell.vars->next, NULL);
		free_vars(g_shell.vars);
		g_shell.vars = NULL;
	}

	TEST_SECTION("ft_setenv 同じ名前の環境変数");
	{
		free_vars(g_shell.vars);
		g_shell.vars = NULL;

		ft_setenv("VAR", "first", 0);
		CHECK_EQ_STR(g_shell.vars->key, "VAR");
		CHECK_EQ_STR(g_shell.vars->value, "first");
		CHECK_EQ(g_shell.vars->is_shell_var, 0);
		CHECK_EQ(g_shell.vars->next, NULL);

		ft_setenv("VAR", "second", 0);
		CHECK_EQ_STR(g_shell.vars->key, "VAR");
		CHECK_EQ_STR(g_shell.vars->value, "second");
		CHECK_EQ(g_shell.vars->is_shell_var, 0);
		CHECK_EQ(g_shell.vars->next, NULL);
		free_vars(g_shell.vars);
		g_shell.vars = NULL;
	}

	TEST_SECTION("ft_setenv 同じ名前のシェル変数");
	{
		free_vars(g_shell.vars);
		g_shell.vars = NULL;

		ft_setenv("VAR", "first", 1);
		CHECK_EQ_STR(g_shell.vars->key, "VAR");
		CHECK_EQ_STR(g_shell.vars->value, "first");
		CHECK_EQ(g_shell.vars->is_shell_var, 1);
		CHECK_EQ(g_shell.vars->next, NULL);

		ft_setenv("VAR", "second", 1);
		CHECK_EQ_STR(g_shell.vars->key, "VAR");
		CHECK_EQ_STR(g_shell.vars->value, "second");
		CHECK_EQ(g_shell.vars->is_shell_var, 1);
		CHECK_EQ(g_shell.vars->next, NULL);
		free_vars(g_shell.vars);
		g_shell.vars = NULL;
	}

	free_vars(g_shell.vars);
	int fail_count = print_result();
	return (fail_count);
}
