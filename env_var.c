#include "libft/libft.h"
#include "minishell.h"
#include "env.h"

void	free_vars(t_var *vars)
{
	t_var	*current;
	t_var	*tmp;

	current = vars;
	while (current)
	{
		free((void *)current->key);
		free((void *)current->value);
		tmp = current;
		current = current->next;
		free(tmp);
	}
}

/* 新しい環境変数(or シェル変数)を追加する
 * 常にkeyでソートされた状態に保たれるように要素が追加される
 */
t_var	*add_new_var(t_var **vars, const char *key, const char *value, bool is_shell_var)
{
	t_var	*current;
	t_var	*prev;
	t_var	*new_var;

	new_var = ft_calloc(1, sizeof(t_var));
	if (!new_var)
		return (NULL);
	new_var->key = ft_strdup(key);
	if (value)
		new_var->value = ft_strdup(value);
	new_var->is_shell_var = is_shell_var;
	if (!*vars)
		*vars = new_var;
	else
	{
		// ABC->ACC->BCC
		//    add AAA
		// AAA->ABC->ACC->BCC
		prev = NULL;
		current = *vars;
		while (current && ft_strcmp(key, current->key) > 0)
		{
			prev = current;
			current = current->next;
		}
		new_var->next = current;
		if (!prev)  // 一番最初
			*vars = new_var;
		else if (!current)  // 一番最後
			prev->next = new_var;
		else  // 途中に追加
			prev->next = new_var;
	}
	return (new_var);
}

/* "key=value" 形式の文字列をt_varに変換する */
t_var	*kvstr2var(char *kvstr, bool is_shell_var)
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
t_var	*environ2vars(char **environ)
{
	int		i;
	t_var	*first;
	t_var	*last;
	t_var	*tmp;

	i = 0;
	first = NULL;
	last = NULL;
	while (environ[i])
	{
		tmp = kvstr2var(environ[i], false);
		if (!tmp)
			put_minish_err_msg_and_exit(1, "environ2t_var", "failed add var");
		if (last)
			last->next = tmp;
		last = tmp;
		if (!first)
			first = last;
		i++;
	}
	return (first);
}

/* t_var 内の環境変数を "key=value" 形式の文字列配列に変換する */
char		**vars2environ(t_var *vars)
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
			env_strarr[i++] = (char *)generate_kvstr(tmp->key, tmp->value);
		tmp = tmp->next;
	}
	return (env_strarr);
}
