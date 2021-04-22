#include "execution.h"

/*
 * free redirection->filepath and redirection object.
 *
 * redirection: t_cmd_redirection object
 */
void	cmd_del_redirection(void *redirection)
{
	const char	*filepath;

	filepath = ((t_cmd_redirection *)redirection)->filepath;
	free((void *)filepath);
	free(redirection);
}

/*
** free command_invocation recursively
*/
void	cmd_free_cmdinvo(t_command_invocation *cmds)
{
	t_command_invocation	*current_cmd;
	t_command_invocation	*prev_cmd;

	if (!cmds)
		return ;
	else
	{
		current_cmd = cmds;
		while (current_cmd)
		{
			ft_lstclear(&current_cmd->input_redirections, cmd_del_redirection);
			ft_lstclear(&current_cmd->output_redirections, cmd_del_redirection);
			free_ptrarr((void **)current_cmd->exec_and_args);
			prev_cmd = current_cmd;
			current_cmd = current_cmd->piped_command;
			free(prev_cmd);
		}
	}
}
