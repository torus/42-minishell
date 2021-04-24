#include "minishell.h"

/* 環境変数を展開する
 */
// 入力例 "hoge$ABC "
char	*expand_env_var(char *str)
{
	int i;
	int start_idx;
	char *result;
	bool is_in_env;

	i = 0;
	start_idx = 0;
	result = NULL;
	is_in_env = false;
	while (i <= (int)ft_strlen(str))
	{
		if (is_in_env && (!(ft_isalnum(str[i]) || str[i] == '_') || !str[i]))
		{
			char *keyname = ft_substr(str, start_idx, i - start_idx);
			if (!keyname)
				return (NULL);
			char *tmp = get_env_val(keyname);
			if (tmp)
			{
				char *tmp_result = result;
				result = ft_strjoin(result, tmp);
				free(tmp);
				free(tmp_result);
			}
			free(keyname);
			start_idx = i;  // ノーマル文字列が始まるidx
			is_in_env = false;
		}
		else if ((str[i] == '$' && (ft_isalnum(str[i + 1]) || str[i + 1] == '_')) || !str[i])  // 環境変数名が始まる!
		{
			char *tmp;
			// ここまでの文字列をresultに格納
			tmp = ft_substr(str, start_idx, i - start_idx);  // '$' を含まない
			if (!tmp)
				return (NULL);
			if (!result)
			{
				result = tmp;
			}else{
				char *tmp2 = result;
				result = ft_strjoin(result, tmp);
				free(tmp);
				free(tmp2);
			}
			start_idx = i + 1;  // 環境変数名の始まるidx
			is_in_env = true;
		}
		i++;
	}
	return (result);
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
	const char	*text;

	if (string_node->type != TOKTYPE_NON_EXPANDABLE
		&& string_node->text[0] == '$' && ft_strlen(string_node->text) > 1)
		text = get_env_val(string_node->text + 1);
	else
		text = ft_strdup(string_node->text);
	splitted_env_val = NULL;
	if (text)
		splitted_env_val = ft_split(text, ' ');
	strarr = (const char **)ptrarr_merge((void **)command->exec_and_args,
			(void **)splitted_env_val);
	free((void *)text);
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
