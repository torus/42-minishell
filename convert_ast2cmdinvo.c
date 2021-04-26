#include "minishell.h"

// string_nodeを文字列に戻す
// うまくいけばASTの時点でこの文字列になるように依頼する
char	*string_node2string(t_parse_node_string *string_node)
{
	char	*result;
	char	*tmp;

	result = ft_strdup("");
	while (string_node)
	{
		if (string_node->type == TOKTYPE_NON_EXPANDABLE)
		{
			tmp = result;
			result = ft_strjoin(result, "\'");
			free(tmp);
		}
		else if (string_node->type == TOKTYPE_EXPANDABLE_QUOTED)
		{
			tmp = result;
			result = ft_strjoin(result, "\"");
			free(tmp);
		}
		tmp = result;
		result = ft_strjoin(result, string_node->text);
		free(tmp);
		if (string_node->type == TOKTYPE_NON_EXPANDABLE)
		{
			tmp = result;
			result = ft_strjoin(result, "\'");
			free(tmp);
		}
		else if (string_node->type == TOKTYPE_EXPANDABLE_QUOTED)
		{
			tmp = result;
			result = ft_strjoin(result, "\"");
			free(tmp);
		}
		if (string_node->next)
			string_node = string_node->next->content.string;
		else
			break;
	}
	return (result);
}

/*
 * 環境変数展開した文字列を分解して返す
 */
char	**split_expanded_str(char *str)
{
	char	**result;
	char	quote_char;  // ' or " or \0

	quote_char = '\0';
	result = NULL;
	// |     hogeabc def"hoge hoge"'$ABC'def abc |
	while (*str)
	{
		// 空白飛ばし
		while (*str && *str == ' ')
			str++;
		if (!*str)
			break ;
		int		len = 0;
		char	*text = NULL;
		while (str[len])
		{
			// クオートの中じゃない時の空白は区切り
			if (!quote_char && str[len] == ' ')
			{
				char *tmp = text;
				char *tmp2 = ft_substr(str, 0, len);
				str += len;
				len = 0;
				if (tmp)
					text = ft_strjoin(tmp, tmp2);
				else
					text = ft_strdup(tmp2);
				free(tmp);
				free(tmp2);
				break;
			}
			if ((str[len] == '\'' || str[len] == '\"') && !(len > 0 && str[len - 1] == '\\'))
			{
				if (quote_char)
				{
					quote_char = '\0';
					char *tmp = text;
					char *tmp2 = ft_substr(str, 0, len);
					str += len + 1;  // クオートの次の文字
					len = 0;
					if (tmp)
						text = ft_strjoin(tmp, tmp2);
					else
						text = ft_strdup(tmp2);
					free(tmp);
					free(tmp2);
				}
				else
				{
					quote_char = str[len];
					char *tmp = text;
					char *tmp2 = ft_substr(str, 0, len);
					str += len + 1;  // クオートの次の文字
					len = 0;
					if (tmp)
						text = ft_strjoin(tmp, tmp2);
					else
						text = ft_strdup(tmp2);
					free(tmp);
					free(tmp2);
				}
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
 * in: abc"x y""abc def"  // abc: expandable, x y: expandable_quoted, abc def: expandable_quoted  // abc"x y""abc def"
 * out: "abcx yabc def"
 *
 * $ABC="abc def"
 * in: "x y"$ABC
 * out: ["x yabc", "def"]
 */
// 一度文字列を全て展開した上でexpanded_strarrを生成する?
char **expand_string_node(t_parse_node_string *string_node)
{
	char	*str;
	char	*tmp;

	// 元の文字列に戻す
	tmp = string_node2string(string_node);  // out: "$ABC"'\\\'$ABC'
	// 元に戻した文字列内の環境変数を展開する
	str = expand_env_var(tmp);  // out: |" abc def "\\'\'$ABC'|
	free(tmp);
	printf("expanded str: |%s|\n", str);
	return (split_expanded_str(str));
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
