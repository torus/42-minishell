#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "execution.h"
#include "minishell.h"
#include "utils.h"

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

int	cmd_add_inredirect(t_command_invocation *command,
	const char *filepath, int fd)
{
	t_cmd_redirection	*red;

	red = ft_calloc(1, sizeof(t_cmd_redirection));
	check_malloc_has_succeeded("add_inredirect", red);
	red->filepath = ft_strdup(filepath);
	check_malloc_has_succeeded("add_inredirect", (void *)red->filepath);
	red->fd = fd;
	if (!cmd_redirection_add_back(command->input_redirections, red))
	{
		free(red);
		return (ERROR);
	}
	return (0);
}

int	cmd_add_outredirect(t_command_invocation *command,
	const char *filepath, int fd, bool is_append)
{
	t_cmd_redirection	*red;

	red = ft_calloc(1, sizeof(t_cmd_redirection));
	check_malloc_has_succeeded("add_outredirect", red);
	red->filepath = ft_strdup(filepath);
	check_malloc_has_succeeded("add_outredirect", (void *)red->filepath);
	red->fd = fd;
	red->is_append = is_append;
	if (!cmd_redirection_add_back(command->output_redirections, red))
	{
		free(red);
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
