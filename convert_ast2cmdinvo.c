#include "parse.h"
#include "execution.h"

/* string node を処理する
** 
** string ::=
**       expandable <no_space> string
**     | expandable
**     | not_expandable <no_space> string
**     | not_expandable
**     | expandable_quoted <no_space> string
**     | expandable_quoted
*/
int cmd_process_string_node(t_parse_node_string *string_node, t_command_invocation *command)
{
}

/* redirection node を処理する
**
** redirection ::=
**       "<" string
**     | ">" string
**     | ">>" string
*/
int cmd_process_redirection_node(t_parse_node_redirection *redirection_node, t_command_invocation *command)
{
	int	redirection_type;

	redirection_type = redirection_node->type;
	if (redirection_type == TOKTYPE_INPUT_REDIRECTION)
		command->input_file_path = redirection_node->string_node->content.string->text;
	else if (redirection_type == TOKTYPE_OUTPUT_REDIRECTION)
	{
		command->output_file_path = redirection_node->string_node->content.string->text;
		command->flags |= CMD_REDIRECT_WRITE;
	}
	else if (redirection_type == TOKTYPE_OUTPUT_APPENDING)
	{
		command->output_file_path = redirection_node->string_node->content.string->text;
		command->flags |= CMD_REDIRECT_APPEND;
	}
	return (0);
}

/* arguments node の string と redirection を処理する
**
** arguments ::=
**       redirection
**     | redirection arguments
**     | string
**     | string arguments
*/
int cmd_process_arguments_node(t_parse_node_arguments *args_node, t_command_invocation *command)
{
	char*	str;

	// string
	if (!ptrarr_add_ptr((void **)command->exec_and_args,
			(void *)args_node->string_node->content.string->text))
		return	(ERROR);
	// redirection
	cmd_process_redirection_node(args_node->redirection_node->content.redirection, command);
	return (0);
}

/*
** command を受け取って t_command_invocation を返す
**
** command ::=
**     arguments
**   | (bonus) "(" sequencial_commands ")"
**   | (bonus) "(" sequencial_commands delimiter ")"
*/
t_command_invocation *cmd_astcmd2cmdinvo(t_parse_node_command *cmd_node)
{
	t_command_invocation	*command;
	t_parse_node_arguments	*args_node;

	command = malloc(sizeof(t_command_invocation));
	if (!command)
		return (NULL);

	// argumentsの処理
	args_node = cmd_node->arguments_node->content.arguments;
	while (args_node)
	{
		cmd_process_arguments_node(args_node, command);
		args_node = args_node->rest_node->content.arguments;
	}
	if (!command->exec_and_args)
		return (NULL);
}

// pipcmds を受け取って t_command_invocation を返す
/*
** pipcmds: piped_commands
** piped_commands ::=
**       command "|" piped_commands
**     | command
*/
int convert_ast_pipcmds2cmdinvo(t_parse_node_pipcmds *pipcmds)
{
	t_command_invocation	*commands;
	t_command_invocation	*command;

	commands = NULL;
	while (pipcmds)
	{
		command = cmd_astcmd2cmdinvo(pipcmds->command_node->content.command);
		cmd_add_cmdinvo(commands, command);
		pipcmds = pipcmds->next->content.piped_commands;
	}
	return (0);
}
