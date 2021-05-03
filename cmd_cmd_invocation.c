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
	cmdinvo->pid = 0;
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
t_command_invocation	*cmd_cmdinvo_add_pipcmd(t_command_invocation **cmds,
	t_command_invocation *pipcmd)
{
	t_command_invocation	*current_cmd;

	if (!*cmds)
		*cmds = pipcmd;
	else
	{
		current_cmd = *cmds;
		while (current_cmd->piped_command)
			current_cmd = current_cmd->piped_command;
		current_cmd->piped_command = pipcmd;
	}
	return (pipcmd);
}
