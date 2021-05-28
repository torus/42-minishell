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

static t_var	*alloc_new_var(const char *key,
	const char *value, bool is_shell_var)
{
	t_var	*new_var;

	new_var = ft_calloc(1, sizeof(t_var));
	if (!new_var)
		return (NULL);
	new_var->key = ft_strdup(key);
	if (value)
		new_var->value = ft_strdup(value);
	new_var->is_shell_var = is_shell_var;
	return (new_var);
}

static t_var	*add_var(t_var **vars, t_var *new_var)
{
	t_var	*current;
	t_var	*prev;

	prev = NULL;
	current = *vars;
	while (current && ft_strcmp(new_var->key, current->key) > 0)
	{
		prev = current;
		current = current->next;
	}
	new_var->next = current;
	if (!prev)
		*vars = new_var;
	else if (!current)
		prev->next = new_var;
	else
		prev->next = new_var;
	return (new_var);
}

/* 新しい環境変数(or シェル変数)を追加する
 * 常にkeyでソートされた状態に保たれるように要素が追加される
 */
t_var	*add_new_var(t_var **vars,
	const char *key, const char *value, bool is_shell_var)
{
	t_var	*new_var;

	new_var = alloc_new_var(key, value, is_shell_var);
	if (!new_var)
		return (NULL);
	if (!*vars)
		*vars = new_var;
	else
		add_var(vars, new_var);
	return (new_var);
}
