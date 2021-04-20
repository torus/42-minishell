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

}

/* arguments node を処理する
**
** arguments ::=
**       redirection
**     | redirection arguments
**     | string
**     | string arguments
*/
int cmd_process_arguments_node(t_parse_node_arguments *args_node, t_command_invocation *command)
{

}

/*
** command を受け取って t_command_invocation を返す
**
** command ::=
**     arguments
**   | (bonus) "(" sequencial_commands ")"
**   | (bonus) "(" sequencial_commands delimiter ")"
*/
t_command_invocation *convert_ast_cmd2cmd(t_parse_node_command *ast_command)
{
	t_command_invocation *command;

	command = malloc(sizeof(t_command_invocation));
	if (!command)
		return (NULL);
	// TODO: argumentsの処理
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
	t_command_invocation *commands;

	commands = NULL;
	while (pipcmds)
	{
		cmd_add_cmdinvo(&commands, convert_ast_cmd2cmd(pipcmds));
		pipcmds = pipcmds->next;
	}
}