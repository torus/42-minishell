#include "libft/libft.h"
#include "minishell.h"
#include "env.h"

/* 新しい環境変数(or シェル変数)を追加する
 * 常にkeyでソートされた状態に保たれるように要素が追加される
 */
t_var	*add_new_var(t_var **vars, t_var *new_var)
{
	t_var	*current;
	t_var	*prev;

	if (!*vars)
		*vars = new_var;
	else
	{
		// ABC->ACC->BCC
		//    add AAA
		// AAA->ABC->ACC->BCC
		prev = NULL;
		current = *vars;
		while (current && ft_strcmp(new_var->key, current->key) > 0)
		{
			prev = current;
			current = current->next;
		}
		if (!prev)  // 一番最初
		{
			new_var->next = current;
			*vars = new_var;
		}
		else if (!current)  // 一番最後
		{
			prev->next = new_var;
		}
		else  // 途中に追加
		{
			prev->next = new_var;
			new_var->next = current;
		}
	}
	return (new_var);
}

/* "key=value" 形式の文字列をt_varに変換する */
t_var	*kvstr2t_var(char *kvstr, bool is_shell_var)
{
	char	**kvarr;
	t_var	*new_var;

	kvarr = split_first_c(kvstr, '=');
	if (!kvarr)
		return (NULL);
	if (!kvarr[0] || !ft_strlen(kvarr[0]))
	{
		free_ptrarr((void **)kvarr);
		return (NULL);
	}
	new_var = ft_calloc(sizeof(t_var), 1);
	new_var->key = kvarr[0];
	new_var->value = kvarr[1];
	new_var->is_shell_var = is_shell_var;
	free(kvarr);
	return (new_var);
}

/* "key=value" 形式の文字列配列をt_varに変換する */
t_var	*environ2t_var(char **environ)
{
	int		i;
	t_var	*vars;

	i = 0;
	while (environ[i])
	{
		if (!add_new_var(&vars, kvstr2t_var(environ[i++], false)))
			put_minish_err_msg_and_exit(1, "environ2t_var", "failed add var");
	}
	return (vars);
}

/* t_var 内の環境変数を "key=value" 形式の文字列配列に変換する */
char		**t_var2environ(t_var *vars)
{
	t_var	*tmp;
	int		vars_len;
	char	**env_strarr;
	int		i;

	vars_len = 0;
	tmp = vars;
	while (tmp)
	{
		if (!tmp->is_shell_var)
			vars_len++;
		tmp = tmp->next;
	}
	env_strarr = ft_calloc(vars_len + 1, sizeof(char *));
	i = 0;
	tmp = vars;
	while (i < vars_len)
	{
		if (!tmp->is_shell_var)
			env_strarr[i++] = generate_kvstr(tmp->key, tmp->value);
		tmp = tmp->next;
	}
	return (env_strarr);
}
