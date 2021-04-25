#include "minishell.h"


/* 
 * in: abc"x y""abc def"
 * out: "abcx y abc def"
 *
 * $ABC="abc def"
 * in: "x y"$ABC
 * out: ["x yabc", "def"]
 */
char **expand_string_node(t_parse_node_string *string_node)
{
	char	**strarr;
	char	*text;
	char	**expanded_strarr;

	while (string_node)
	{
		if (string_node->type == TOKTYPE_EXPANDABLE
				|| string_node->type == TOKTYPE_EXPANDABLE_QUOTED)
		{
			text = expand_env_var(string_node->text);  // $ABC => "abc def"
			if (text && string_node->type == TOKTYPE_EXPANDABLE)
			{
				strarr = ft_split(text, ' ');
				// expanded_strarrの最後とstrarrの最初をくっつける
				char *tmp = expanded_strarr[ptrarr_len((void **)expanded_strarr) - 1];
				char *tmp2 = strarr[0];
				expanded_strarr[ptrarr_len((void **)expanded_strarr) - 1] = ft_strjoin(expanded_strarr[ptrarr_len((void **)expanded_strarr) - 1], strarr[0]);
				free(tmp);
				free(tmp2);
				// strarrの残りの部分をexpanded_strarrに追加する
				int i = 1;
				while (strarr[i])
				{
					char **tmp = expanded_strarr;
					expanded_strarr = (char **)ptrarr_add_ptr((void **)expanded_strarr, strarr[i++]);
					free(tmp);
				}
				free(strarr);
			}
			else if (text && string_node->type == TOKTYPE_EXPANDABLE_QUOTED)
			{
				expanded_strarr = (char **)ptrarr_add_ptr((void**)expanded_strarr, text);
			}
		}
		else
		{
			text = ft_strdup(string_node->text);
			expanded_strarr = (char **)ptrarr_add_ptr((void**)expanded_strarr, text);
		}
		if (string_node->next)
			string_node = string_node->next->content.string;
		else
			break;
	}
	return (expanded_strarr);
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
	char		**splitted_env_val;
	const char	**strarr;

	splitted_env_val = expand_string_node(string_node);
	strarr = (const char **)ptrarr_merge((void **)command->exec_and_args,
			(void **)splitted_env_val);
	free(splitted_env_val);
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
		cmd_add_inredirect(command, text);
	else if (redirection_type == TOKTYPE_OUTPUT_REDIRECTION)
		cmd_add_outredirect(command, text, false);
	else if (redirection_type == TOKTYPE_OUTPUT_APPENDING)
		cmd_add_outredirect(command, text, true);
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

	command = cmd_init_cmdinvo(NULL);
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
		cmd_cmdinvo_add_pipcmd(&commands, command);
		if (pipcmds->next)
			pipcmds = pipcmds->next->content.piped_commands;
		else
			pipcmds = NULL;
	}
	return (commands);
}
