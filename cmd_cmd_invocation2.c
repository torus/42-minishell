#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "minishell.h"
#include "execution.h"
#include "utils.h"

/*
 * free redirection->filepath and redirection object.
 *
 * redirection: t_cmd_redirection object
 */
void	cmd_free_redirection(t_cmd_redirection *redirection)
{
	free((void *)redirection->filepath);
	free(redirection);
}

void	cmd_free_redirections(t_cmd_redirection *redirections)
{
	t_cmd_redirection *tmp;

	while (redirections)
	{
		tmp = redirections;
		redirections = redirections->next;
		cmd_free_redirection(tmp);
	}
}

/*
** free command_invocation recursively
*/
void	cmd_free_cmdinvo(t_command_invocation *cmds)
{
	t_command_invocation	*current_cmd;
	t_command_invocation	*prev_cmd;

	if (!cmds)
		return ;
	else
	{
		current_cmd = cmds;
		while (current_cmd)
		{
			cmd_free_redirections(current_cmd->input_redirections);
			cmd_free_redirections(current_cmd->output_redirections);
			free_ptrarr((void **)current_cmd->exec_and_args);
			prev_cmd = current_cmd;
			current_cmd = current_cmd->piped_command;
			free(prev_cmd);
		}
	}
}

static void	readline4heredoc(t_cmd_redirection *red, const char *limit_str)
{
	char	*input_str;

	rl_event_hook = cmd_check_readline_has_finished;
	while (!g_shell.heredoc_interruption)
	{
		input_str = readline("> ");
		if (!input_str || !ft_strcmp(input_str, limit_str))
			break ;
		red->filepath = strjoin_nullable_and_free_both(
				(char *)red->filepath, input_str);
		check_malloc_has_succeeded("heredoc", (void *)red->filepath);
		red->filepath = strjoin_and_free_first(
				(char *)red->filepath, "\n");
		check_malloc_has_succeeded("heredoc", (void *)red->filepath);
	}
	rl_event_hook = NULL;
	if (input_str)
		free(input_str);
	else
		write(1, "\n", 1);
}

int	cmd_add_heredoc(t_command_invocation *command,
	const char *limit_str, int fd, bool is_expandable)
{
	t_cmd_redirection	*red;
	const char			*old_filepath;

	red = ft_calloc(1, sizeof(t_cmd_redirection));
	check_malloc_has_succeeded("heredoc", (void *)red);
	red->fd = fd;
	red->is_heredoc = true;
	cmd_set_heredoc_sighandlers();
	readline4heredoc(red, limit_str);
	if (g_shell.heredoc_interruption)
	{
		cmd_free_redirection(red);
		return (ERROR);
	}
	if (is_expandable)
	{
		old_filepath = red->filepath;
		red->filepath = expand_heredoc_document((char *)red->filepath);
		free((void *)old_filepath);
	}
	if (!cmd_redirection_add_back(&command->input_redirections, red))
		put_minish_err_msg_and_exit(1, "heredoc", "lstadd_back failed");
	set_sighandlers_during_execution();
	return (0);
}
