#include "minishell.h"

/*
 * return splitted environment variable's value.
 *
 * env_key: keyname of environment variable without '$'
 *
 * return: splitted value of environment variable with space.
 *         If env_key isn't found, return ft_split("", ' ').
 */
char	**cmd_expand_env_var(char *env_key)
{
	char	*env_val;
	char	**splitted_val;

	env_val = get_env_val(env_key);
	if (!env_val)
		return (ft_split("", ' '));
	splitted_val = ft_split(env_val, ' ');
	free(env_val);
	if (!splitted_val)
		return (NULL);
	return (splitted_val);
}

/* set values of command->exec_and_args based on string_node
**
** string ::=
**       expandable <no_space> string
**     | expandable
**     | not_expandable <no_space> string
**     | not_expandable
**     | expandable_quoted <no_space> string
**     | expandable_quoted
*/
int	cmd_process_string_node(t_parse_node_string *string_node,
	t_command_invocation *command)
{
	const char	**strarr;
	const char	*text;

	// 環境変数の展開
	if (string_node->type != TOKTYPE_NON_EXPANDABLE
			&& (string_node->text[0] == '$' && ft_strlen(string_node->text) > 1))
	{
		char **splitted_env_val = cmd_expand_env_var(string_node->text + 1);
		strarr = (const char **)ptrarr_merge((void **)command->exec_and_args,
			(void **)splitted_env_val);
		free(splitted_env_val);
	}
	else
	{
		text = ft_strdup(string_node->text);
		if (!text)
			return (ERROR);
		strarr = (const char **)ptrarr_add_ptr((void **)command->exec_and_args,
			(void *)text);
	}
	if (!strarr)
		return (ERROR);
	free((void **)command->exec_and_args);
	command->exec_and_args = strarr;
	return (0);
}

/* set values of command->input_file_path or output_file_path
** based on redirection_node
**
** redirection ::=
**       "<" string
**     | ">" string
**     | ">>" string
*/
int	cmd_process_redirection_node(t_parse_node_redirection *redirection_node,
	t_command_invocation *command)
{
	int			redirection_type;
	const char	*text;

	redirection_type = redirection_node->type;
	text = ft_strdup(redirection_node->string_node->content.string->text);
	if (!text)
		return (ERROR);
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

/* parse argument_node and set values of command
**
** arguments ::=
**       redirection
**     | redirection arguments
**     | string
**     | string arguments
*/
int	cmd_process_arguments_node(t_parse_node_arguments *args_node,
	t_command_invocation *command)
{
	if (args_node->string_node)
		cmd_process_string_node(
			args_node->string_node->content.string, command);
	else if (args_node->redirection_node)
		cmd_process_redirection_node(
			args_node->redirection_node->content.redirection, command);
	else
		return (1);
	return (0);
}

/* convert ast command_node to command_invocation object
**
** command ::=
**     arguments
**   | (bonus) "(" sequencial_commands ")"
**   | (bonus) "(" sequencial_commands delimiter ")"
*/
t_command_invocation	*cmd_ast_cmd2cmdinvo(t_parse_node_command *cmd_node)
{
	t_command_invocation	*command;
	t_parse_node_arguments	*args_node;

	command = cmd_init_cmdinvo(NULL, NULL, NULL, 0);
	if (!command)
		return (NULL);
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

/* convert ast command_node to command_invocation object
**
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
