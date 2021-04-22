#include "execution.h"

/*
** Malloc and initialize t_command_invocation
*/
t_command_invocation	*cmd_init_cmdinvo(const char *output_file_path,
	const char *input_file_path, const char **exec_and_args, unsigned int flags)
{
	t_command_invocation	*cmdinvo;

	cmdinvo = malloc(sizeof(t_command_invocation));
	if (!cmdinvo)
		return (NULL);
	cmdinvo->input_file_path = input_file_path;
	cmdinvo->output_file_path = output_file_path;
	cmdinvo->exec_and_args = exec_and_args;
	cmdinvo->piped_command = NULL;
	cmdinvo->flags = flags;
	return (cmdinvo);
}

/*
** cmd->pipe_command にコマンドを追加
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
			free((void *)current_cmd->input_file_path);
			free((void *)current_cmd->output_file_path);
			free_ptrarr((void **)current_cmd->exec_and_args);
			prev_cmd = current_cmd;
			current_cmd = current_cmd->piped_command;
			free(prev_cmd);
		}
	}
}
