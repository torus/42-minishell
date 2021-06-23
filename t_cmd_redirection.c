#include "execution.h"

t_cmd_redirection	*cmd_redirection_add_back(t_cmd_redirection **reds,
	t_cmd_redirection *new_red)
{
	t_cmd_redirection	*red;

	new_red->next = NULL;
	red = *reds;
	if (!red)
		*reds = new_red;
	else
	{
		while (red->next)
			red = red->next;
		red->next = new_red;
	}
	return (new_red);
}

t_cmd_redirection	*cmd_copy_redirection(t_cmd_redirection *original)
{
	t_cmd_redirection	*new;

	new = malloc(sizeof(t_cmd_redirection));
	if (!new)
		return (NULL);
	new->fd = original->fd;
	new->filepath = ft_strdup(original->filepath);
	if (!new->filepath)
		return (NULL);
	new->is_append = original->is_append;
	new->is_heredoc = original->is_heredoc;
	new->next = NULL;
	return (new);
}
