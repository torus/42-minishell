#include "execution.h"

t_cmd_redirection	*cmd_redirection_add_back(t_cmd_redirection *reds, t_cmd_redirection *new_red)
{
	while (reds->next)
		reds = reds->next;
	new_red->next = NULL;
	reds->next = new_red;
	return (new_red);
}
