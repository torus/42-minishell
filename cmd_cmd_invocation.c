#include "execution.h"

/*
** Malloc and initialize t_command_invocation
*/
t_command_invocation	*cmd_init_cmdinvo(const char **exec_and_args)
{
	t_command_invocation	*cmdinvo;

	cmdinvo = malloc(sizeof(t_command_invocation));
	if (!cmdinvo)
		return (NULL);
	cmdinvo->exec_and_args = exec_and_args;
	cmdinvo->input_redirections = NULL;
	cmdinvo->output_redirections = NULL;
	cmdinvo->piped_command = NULL;
	return (cmdinvo);
}

/*
 * add input redirection
 */
int	cmd_add_inredirect(t_command_invocation *command,
	const char *filepath)
{
	t_cmd_redirection	*redirection;

	redirection = ft_calloc(1, sizeof(t_cmd_redirection));
	redirection->filepath = filepath;
	if (!redirection || !ft_lstadd_back_new(
			&command->input_redirections, (void *)redirection))
	{
		free(redirection);
		return (ERROR);
	}
	return (0);
}

/*
 * add output redirection
 */
int	cmd_add_outredirect(t_command_invocation *command,
	const char *filepath, bool is_append)
{
	t_cmd_redirection	*redirection;

	redirection = ft_calloc(1, sizeof(t_cmd_redirection));
	redirection->filepath = filepath;
	redirection->is_append = is_append;
	if (!redirection || !ft_lstadd_back_new(
			&command->output_redirections, (void *)redirection))
	{
		free(redirection);
		return (ERROR);
	}
	return (0);
}

/*
** add command to cmd->pipe_command
*/
t_command_invocation	*cmd_add_cmdinvo(t_command_invocation **cmds,
	t_command_invocation *newcmd)
{
	t_command_invocation	*current_cmd;

	if (!*cmds)
		*cmds = newcmd;
	else
	{
		current_cmd = *cmds;
		while (current_cmd->piped_command)
			current_cmd = current_cmd->piped_command;
		current_cmd->piped_command = newcmd;
	}
	return (newcmd);
}

/*
 * free redirection->filepath and redirection object.
 *
 * redirection: t_cmd_redirection object
 */
void	del_redirection(void *redirection)
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
			ft_lstclear(&current_cmd->input_redirections, del_redirection);
			ft_lstclear(&current_cmd->output_redirections, del_redirection);
			free_ptrarr((void **)current_cmd->exec_and_args);
			prev_cmd = current_cmd;
			current_cmd = current_cmd->piped_command;
			free(prev_cmd);
		}
	}
}
