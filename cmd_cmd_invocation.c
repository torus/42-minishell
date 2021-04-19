#include "execution.h"

/*
** Malloc and initialize t_command_invocation
*/
t_command_invocation	*cmd_init_cmdinvo(const char *output_file_path,
	const char *input_file_path, const char **exec_and_args )
{
	t_command_invocation *cmdinvo;

	cmdinvo = malloc(sizeof(t_command_invocation));
	if (!cmdinvo)
		return (NULL);
	cmdinvo->input_file_path = input_file_path;
	cmdinvo->output_file_path = output_file_path;
	cmdinvo->exec_and_args = exec_and_args;
	cmdinvo->piped_command = NULL;
	return (cmdinvo);
}

/*
** cmd->pipe_command にコマンドを追加
*/
t_command_invocation *add_cmdinvo(t_command_invocation *cmds, t_command_invocation *newcmd)
{
	while (cmds->piped_command)
		cmds = cmds->piped_command;
	cmds->piped_command = newcmd;
	return (newcmd);
}