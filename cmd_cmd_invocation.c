#include "execution.h"

/*
** Malloc and initialize t_command_invocation
*/
t_command_invocation *init_cmdinvo(const char *output_file_path,
	const char *input_file_path, const char **exec_and_args )
{
	t_command_invocation *cmdinvo;
	cmdinvo = malloc(sizeof(t_command_invocation));
	if (!cmdinvo)
		return (NULL);
	cmdinvo->input_file_path = input_file_path;
	cmdinvo->output_file_path = output_file_path;
	cmdinvo->exec_and_args = exec_and_args;
	return (cmdinvo);
}