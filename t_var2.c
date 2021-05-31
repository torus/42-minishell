#include "libft/libft.h"
#include "minishell.h"
#include "env.h"

/* Convert a string that's format "key=value" to t_var. */
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

/* Convert array of string that's format "key=value" to t_var */
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

/* Convert t_var to a string that's format "key=value" */
char	**vars2environ(t_var *vars)
{
	t_var	*tmp;
	int		vars_len;
	char	**env_strarr;
	int		i;

	vars_len = 0;
	tmp = vars;
	while (tmp)
	{
		if (!tmp->is_shell_var && tmp->value)
			vars_len++;
		tmp = tmp->next;
	}
	env_strarr = ft_calloc(vars_len + 1, sizeof(char *));
	i = 0;
	tmp = vars;
	while (i < vars_len)
	{
		if (!tmp->is_shell_var && tmp->value)
			env_strarr[i++] = (char *)generate_kvstr(tmp->key, tmp->value);
		tmp = tmp->next;
	}
	return (env_strarr);
}
