#include "minishell.h"

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
	const char **strarr = (const char **)ptrarr_add_ptr(
		(void **)command->exec_and_args, (void *)string_node->text);
	if (!strarr)
		return	(ERROR);
	free((void **)command->exec_and_args);
	command->exec_and_args = strarr;
	return (0);
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
	const char	*text;

	redirection_type = redirection_node->type;
	text = redirection_node->string_node->content.string->text;
	if (redirection_type == TOKTYPE_INPUT_REDIRECTION)
		command->input_file_path = text;
	else if (redirection_type == TOKTYPE_OUTPUT_REDIRECTION)
	{
		command->output_file_path = text;
		command->flags |= CMD_REDIRECT_WRITE;
	}
	else if (redirection_type == TOKTYPE_OUTPUT_APPENDING)
	{
		command->output_file_path = text;
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
	// string
	if (args_node->string_node)
		cmd_process_string_node(args_node->string_node->content.string, command);
	// redirection
	else if (args_node->redirection_node)
		cmd_process_redirection_node(args_node->redirection_node->content.redirection, command);
	else
		return (1);
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
t_command_invocation *cmd_ast_cmd2cmdinvo(t_parse_node_command *cmd_node)
{
	t_command_invocation	*command;
	t_parse_node_arguments	*args_node;

	command = cmd_init_cmdinvo(NULL, NULL, NULL, 0);
	if (!command)
		return (NULL);

	// argumentsの処理
	args_node = cmd_node->arguments_node->content.arguments;
	while (args_node)
	{
		cmd_process_arguments_node(args_node, command);
		if (args_node->rest_node)
			args_node = args_node->rest_node->content.arguments;
		else
			args_node = NULL;
	}
	if (!command->exec_and_args)
		return (NULL);
	return (command);
}

// pipcmds を受け取って t_command_invocation を返す
/*
** pipcmds: piped_commands
** piped_commands ::=
**       command "|" piped_commands
**     | command
*/
t_command_invocation	*cmd_ast_pipcmds2cmdinvo(t_parse_node_pipcmds *pipcmds)
{
	t_command_invocation	*commands;
	t_command_invocation	*command;

	commands = NULL;
	while (pipcmds)
	{
		command = cmd_ast_cmd2cmdinvo(pipcmds->command_node->content.command);
		cmd_add_cmdinvo(&commands, command);
		if (pipcmds->next)
			pipcmds = pipcmds->next->content.piped_commands;
		else
			pipcmds = NULL;
	}
	return (commands);
}
