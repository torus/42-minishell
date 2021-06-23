#include "execution.h"

t_cmd_redirection	*cmd_redirection_add_back(t_cmd_redirection **reds, t_cmd_redirection *new_red)
{
	new_red->next = NULL;
	if (!*reds)
	{
		*reds = new_red;
		return (new_red);
	}
	while ((*reds)->next)
		(*reds) = (*reds)->next;
	(*reds)->next = new_red;
	return (new_red);
}
