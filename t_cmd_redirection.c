#include "execution.h"

t_cmd_redirection	*cmd_redirection_add_back(t_cmd_redirection **reds, t_cmd_redirection *new_red)
{
	t_cmd_redirection	*red;

	new_red->next = NULL;
	if (!*reds)
	{
		*reds = new_red;
		return (new_red);
	}
	red = *reds;
	while (red->next)
		red = red->next;
	red->next = new_red;
	return (new_red);
}
