#include "test.h"
#include "../env.h"
#include "../libft/libft.h"
#include "../minishell.h"

t_shell	g_shell;

void init_g_shell(void)
{
	g_shell.cwd = NULL;
}

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
	extern char **environ;

	TEST_SECTION("split_first_c() 普通に\"Key=Value\"");
	{
		char *input = "PATH=/bin/:/usr/bin/:/home/jun/bin";
		char **str_arr = split_first_c(input, '=');
		CHECK_EQ_STR(str_arr[0], "PATH");
		CHECK_EQ_STR(str_arr[1], "/bin/:/usr/bin/:/home/jun/bin");
		CHECK_EQ(str_arr[2], NULL);

		free_ptrarr((void **)str_arr);
	}

	TEST_SECTION("split_first_c() 区切り文字が入っていない");
	{
		char *input = "PATH/bin/:/usr/bin/:/home/jun/bin";
		char **str_arr = split_first_c(input, '=');
		CHECK_EQ_STR(str_arr[0], "PATH/bin/:/usr/bin/:/home/jun/bin");
		CHECK_EQ(str_arr[1], NULL);
		CHECK_EQ(str_arr[2], NULL);

		free_ptrarr((void **)str_arr);
	}

	TEST_SECTION("split_first_c() 最後に区切り文字\"key=\"");
	{
		char *input = "PATH=";
		char **str_arr = split_first_c(input, '=');
		CHECK_EQ_STR(str_arr[0], "PATH");
		CHECK_EQ_STR(str_arr[1], "");
		CHECK_EQ(str_arr[2], NULL);

		free_ptrarr((void **)str_arr);
	}

	TEST_SECTION("get_colon_units() \":/hoge/::/\"");
	{
		char *input = ":/hoge/::/";
		char *default_str = "";
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
		char *input = "/hoge/::/::";
		char *default_str = "./";
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
		char *original0 = environ[0];
		environ[0] = "GET_ENV_TEST=/bin/:/usr/bin/:/home/jun/bin";
		char *kvstr = get_env("GET_ENV_TEST");
		CHECK_EQ_STR(kvstr, environ[0]);
		environ[0] = original0;
	}

	TEST_SECTION("get_env() 指定した環境変数が存在しない場合");
	{
		char *kvstr = get_env("THIS_IS_NOT_EXIST_IN_ENVIRON");
		CHECK_EQ(kvstr, NULL);
	}

	TEST_SECTION("get_env() 途中まで同じ環境変数名な時");
	{
		char *original0 = environ[0];
		environ[0] = "GET_ENV_TEST_TEST=/bin/:/usr/bin/:/home/jun/bin";
		char *kvstr = get_env("GET_ENV_TEST");
		CHECK_EQ(kvstr, NULL);
		environ[0] = original0;
	}

	TEST_SECTION("get_val_from_kvstr() 通常ケース");
	{
		char *kvstr = "PATH=/bin/:/usr/bin/:/home/jun/bin";
		char *val_str = get_val_from_kvstr(kvstr, '=');
		CHECK_EQ_STR(val_str, "/bin/:/usr/bin/:/home/jun/bin");

		free(val_str);
	}

	TEST_SECTION("get_val_from_kvstr() 区切り文字が無い");
	{
		char *kvstr = "PATH/bin/:/usr/bin/:/home/jun/bin";
		char *val_str = get_val_from_kvstr(kvstr, '=');
		CHECK_EQ(val_str, NULL);

		free(val_str);
	}

	TEST_SECTION("get_val_from_kvstr() 区切り文字が連続で並ぶ");
	{
		char *kvstr = "PATH===/bin/:/usr/bin/:/home/jun/bin";
		char *val_str = get_val_from_kvstr(kvstr, '=');
		CHECK_EQ_STR(val_str, "==/bin/:/usr/bin/:/home/jun/bin");

		free(val_str);
	}

	TEST_SECTION("get_env_val() 通常ケース");
	{
		char *original0 = environ[0];
		environ[0] = "GET_ENV_TEST=/bin/:/usr/bin/:/home/jun/bin";
		char *val_str = get_env_val("GET_ENV_TEST");
		CHECK_EQ_STR(val_str, "/bin/:/usr/bin/:/home/jun/bin");
		environ[0] = original0;

		free(val_str);
	}

	TEST_SECTION("get_env_val() 指定された環境変数が存在しない");
	{
		char *val_str = get_env_val("THIS_IS_NOT_EXIST_IN_ENVIRON");
		CHECK_EQ(val_str, NULL);

		free(val_str);
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
		char	*prev = getenv("ABC");
		CHECK(ft_setenv("ABC", "ft_setenv()", 0) == 0);
		char	*new = getenv("ABC");
		CHECK_EQ_STR(new, prev);
		ft_unsetenv("ABC");
	}

	TEST_SECTION("ft_setenv() update rewrite=1");
	{
		ft_setenv("ABC", "ft_setenv() update rewrite=1", 1);
		char	*prev = getenv("ABC");
		CHECK(prev);
		CHECK(ft_setenv("ABC", "ft_setenv()", 1) == 0);
		char	*new = getenv("ABC");
		CHECK_EQ_STR(new, "ft_setenv()");
		ft_unsetenv("ABC");
	}

	TEST_SECTION("ft_setenv() expand_and_add_env");
	{
		ft_unsetenv("ABC");
		char	*prev = getenv("ABC");
		CHECK(!prev);
		CHECK(ft_setenv("ABC", "ft_setenv()", 1) == 0);
		char	*new = getenv("ABC");
		CHECK_EQ_STR(new, "ft_setenv()");
	}

	TEST_SECTION("ft_unsetenv() unset env");
	{
		ft_setenv("ABC", "ft_unsetenv() unset env", 1);
		char	*prev = getenv("ABC");
		CHECK(prev);
		CHECK(ft_unsetenv("ABC") == 0);
		char	*new = getenv("ABC");
		CHECK(!new);
		ft_unsetenv("ABC");
	}

	TEST_SECTION("ft_unsetenv() unset env that doesn't exist");
	{
		ft_unsetenv("ABC");
		char	*prev = getenv("ABC");
		CHECK(!prev);
		CHECK(ft_unsetenv("ABC") == 0);
		char	*new = getenv("ABC");
		CHECK(!new);
		ft_unsetenv("ABC");
	}

	TEST_CHAPTER("t_var");

	TEST_SECTION("kvstr2t_var");
	{
		char *kvstr = "env1=this is env1";
		t_var *var = kvstr2t_var(kvstr, 0);
		CHECK(var);
		CHECK_EQ_STR(var->key, "env1");
		CHECK_EQ_STR(var->value, "this is env1");
		CHECK_EQ(var->is_shell_var, 0);
		CHECK_EQ(var->next, NULL);
	}

	TEST_SECTION("文字列配列をt_varに変換する");
	{
		char	**envarr;
		t_var	*vars;

		envarr = ft_split("env1=this_is_env1 env2=this_is_env2 env3=this_is_env3", ' ');

		vars = environ2t_var(envarr);
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
	}

	TEST_SECTION("文字列配列をt_varに変換して変数を追加し, 文字列配列に戻す");
	{
		char	**envarr_expected;
		char	**envarr_actual;
		t_var	*vars;
		t_var	*new_var;

		envarr_expected = ft_split("env1=env1 env2=env2 env3=env3", ' ');

		vars = environ2t_var(envarr_expected);
		new_var = ft_calloc(1, sizeof(t_var));
		new_var->key = ft_strdup("NEW_ENV");
		new_var->value = ft_strdup("new env");
		new_var->is_shell_var = 0;
		add_new_var(&vars, new_var);

		envarr_actual = t_var2environ(vars);
		CHECK(envarr_actual);

		char	**tmp = envarr_expected;
		envarr_expected = (char **)ptrarr_add_ptr((void **)envarr_expected, ft_strdup("NEW_ENV=new env"));
		free(tmp);

		sort_strarr(envarr_actual);
		sort_strarr(envarr_expected);
		check_strarr((const char**)envarr_actual, (const char**)envarr_expected);
	}

	char c;
	printf("environ: %p\n", environ);
	printf("stack:   %p\n", &c);
	// environ が heap領域 に配置されている時はfreeする
	if ((void *)environ < (void *)&c)
		free(environ);

	int fail_count = print_result();
	return (fail_count);
}
