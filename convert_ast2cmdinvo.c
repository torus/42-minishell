#include "minishell.h"

/* set values of command->exec_and_args based on string_node
 *
 * string ::=
 *       expandable <no_space> string
 *     | expandable
 *     | not_expandable <no_space> string
 *     | not_expandable
 *     | expandable_quoted <no_space> string
 *     | expandable_quoted
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
 * based on redirection_node
 *
 * To follow to bash behavior, environment variable expansion is not done here,
 *   but is done just before execution.
 *
 * redirection ::=
 *       "<" string
 *     | ">" string
 *     | ">>" string
 */
#include <stdio.h>
int	cmd_process_redirection_node(t_parse_node_redirection *redirection_node,
	t_command_invocation *command)
{
	int			redirection_type;
	const char	*text;
	bool		is_expandable_heredoc;
	int			fd;
	int			status;

	redirection_type = redirection_node->type;
	text = string_node2string(redirection_node->string_node->content.string, redirection_type != TOKTYPE_HEREDOCUMENT);

	// heredoc が expandable かどうか取得する
	t_parse_node_string *str_node = redirection_node->string_node->content.string;
	is_expandable_heredoc = 1;
	while (str_node)
	{
		if (str_node->type == TOKTYPE_NON_EXPANDABLE)
			is_expandable_heredoc = 0;
		if (str_node->next)
			str_node = str_node->next->content.string;
		else
			str_node = NULL;
	}

	printf("text: |%s|, is_expandable: %d\n", text, is_expandable_heredoc);

	fd = redirection_node->fd;
	status = 0;
	if (!text)
		return (ERROR);
	if (redirection_type == TOKTYPE_INPUT_REDIRECTION)
		status = cmd_add_inredirect(command, text, fd);
	else if (redirection_type == TOKTYPE_OUTPUT_REDIRECTION)
		status = cmd_add_outredirect(command, text, fd, false);
	else if (redirection_type == TOKTYPE_OUTPUT_APPENDING)
		status = cmd_add_outredirect(command, text, fd, true);
	else if (redirection_type == TOKTYPE_HEREDOCUMENT)
		status = cmd_add_heredoc(command, text, fd, is_expandable_heredoc);
	free((void *)text);
	return (status);
}

/* parse argument_node and set values of command
 *
 * arguments ::=
 *       redirection
 *     | redirection arguments
 *     | string
 *     | string arguments
 */
int	cmd_process_arguments_node(t_parse_node_arguments *args_node,
	t_command_invocation *command)
{
	if (args_node->string_node)
		return (cmd_process_string_node(
			args_node->string_node->content.string, command));
	else if (args_node->redirection_node)
		return (cmd_process_redirection_node(
			args_node->redirection_node->content.redirection, command));
	else
		return (1);
	return (0);
}

/* convert ast command_node to command_invocation object
 *
 * command ::=
 *     arguments
 *   | (bonus) "(" sequencial_commands ")"
 *   | (bonus) "(" sequencial_commands delimiter ")"
 */
t_command_invocation	*cmd_ast_cmd2cmdinvo(t_parse_node_command *cmd_node)
{
	t_command_invocation	*cmdinvo;
	t_parse_node_arguments	*args_node;

	cmdinvo = cmd_init_cmdinvo(NULL);
	if (!cmdinvo)
		put_minish_err_msg_and_exit(1, "ast2cmdinvo", "malloc() failed");
	args_node = cmd_node->arguments_node->content.arguments;
	while (args_node)
	{
		if (cmd_process_arguments_node(args_node, cmdinvo))
		{
			cmd_free_cmdinvo(cmdinvo);
			return (NULL);
		}
		if (args_node->rest_node)
			args_node = args_node->rest_node->content.arguments;
		else
			args_node = NULL;
	}
	return (cmdinvo);
}

/* convert ast command_node to command_invocation object
 *
 * pipcmds: piped_commands
 * piped_commands ::=
 *       command "|" piped_commands
 *     | command
 */
t_command_invocation	*cmd_ast_pipcmds2cmdinvo(t_parse_node_pipcmds *pipcmds)
{
	t_command_invocation	*commands;
	t_command_invocation	*command;

	commands = NULL;
	while (pipcmds)
	{
		command = cmd_ast_cmd2cmdinvo(pipcmds->command_node->content.command);
		if (!command)
		{
			cmd_free_cmdinvo(commands);
			return (NULL);
		}
		cmd_cmdinvo_add_pipcmd(&commands, command);
		if (pipcmds->next)
			pipcmds = pipcmds->next->content.piped_commands;
		else
			pipcmds = NULL;
	}
	return (commands);
}
