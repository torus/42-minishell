#include "minishell.h"

/*
 * first に second を繋げて, firstだけfreeする
 */
static char	*strjoin_and_free_first(char *first, char *second)
{
	char	*tmp;

	tmp = first;
	first = ft_strjoin(first, second);
	free(tmp);
	return (first);
}

/* string_nodeを文字列に戻す
 *うまくいけばASTの時点でこの文字列になるように依頼する
 */
char	*string_node2string(t_parse_node_string *string_node)
{
	char	*result;

	result = ft_strdup("");
	while (string_node)
	{
		if (string_node->type == TOKTYPE_NON_EXPANDABLE)
			result = strjoin_and_free_first(result, "\'");
		else if (string_node->type == TOKTYPE_EXPANDABLE_QUOTED)
			result = strjoin_and_free_first(result, "\"");
		result = strjoin_and_free_first(result, string_node->text);
		if (string_node->type == TOKTYPE_NON_EXPANDABLE)
			result = strjoin_and_free_first(result, "\'");
		else if (string_node->type == TOKTYPE_EXPANDABLE_QUOTED)
			result = strjoin_and_free_first(result, "\"");
		if (string_node->next)
			string_node = string_node->next->content.string;
		else
			break ;
	}
	return (result);
}

/*
 * from から len文字substrして dst にくっつける
 */
static char	*substr_and_join(char *dst, char *from, int len)
{
	char *tmp_dst;
	char *tmp_substr;

	tmp_dst = dst;
	tmp_substr = ft_substr(from, 0, len);
	if (dst)
		dst = ft_strjoin(dst, tmp_substr);
	else
		dst = ft_strdup(tmp_substr);
	free(tmp_dst);
	free(tmp_substr);
	return (dst);
}

/*
 * 環境変数展開した文字列を分解して返す
 */
char	**split_expanded_str(char *str)
{
	char	**result;
	char	quote_char;
	int		len;
	char	*text;

	quote_char = '\0';
	result = NULL;
	while (*str)
	{
		// 空白飛ばし
		while (*str && *str == ' ')
			str++;
		if (!*str)
			break ;
		len = 0;
		text = NULL;
		while (true)
		{
			// クオートの中じゃない時の空白は区切り
			if ((!quote_char && str[len] == ' ') || !str[len])
			{
				text = substr_and_join(text, str, len);
				str += len;
				break;
			}
			if ((str[len] == '\'' || str[len] == '\"') && !(len > 0 && str[len - 1] == '\\'))
			{
				if (quote_char)
					quote_char = '\0';
				else
					quote_char = str[len];
				text = substr_and_join(text, str, len);
				str += len + 1;  // クオートの次の文字
				len = 0;
			}
			else
				len++;
		}
		char	**tmp = result;
		result = (char **)ptrarr_add_ptr((void **)tmp, text);
		free(tmp);
	}
	return (result);
}

/*
 * 一度文字列を全て展開した上でexpanded_strarrを生成する
 */
char	**expand_string_node(t_parse_node_string *string_node)
{
	char	*restored_str;
	char	*expanded_str;
	char	**splitted_expanded_str;

	restored_str = string_node2string(string_node);
	expanded_str = expand_env_var(restored_str);
	free(restored_str);
	splitted_expanded_str = split_expanded_str(expanded_str);
	free(expanded_str);
	return (splitted_expanded_str);
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
