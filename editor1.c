#include <unistd.h>
#include <stdio.h>

#include "rope.h"
#include "editor.h"
#include "parse.h"
#include "minishell.h"

t_rope	*edit_get_line(t_command_history *history, t_command_state *state)
{
	char	cbuf[2];
	t_rope	*rope;
	int		input_count;

	rope = NULL;
	cbuf[1] = '\0';
	while (1)
	{
		input_count = 0;
		while (input_count == 0)
		{
			input_count = read(STDIN_FILENO, cbuf, 1);
			if (g_shell.interrupted)
			{
				g_shell.interrupted = 0;
				splay_assign(&history->ropes[history->current], NULL);
				splay_release(rope);
				edit_putc('\n');
				return (NULL);
			}
		}
		if (input_count != 1)
			break ;
		if (cbuf[0] >= 0x20)
			edit_normal_character(history, state, cbuf);
		else if (cbuf[0] == '\n')
		{
			splay_assign(&rope, history->ropes[history->current]);
			edit_enter(history, state);
			if (rope)
				rope->refcount--;
			return (rope);
		}
		else if (cbuf[0] == 0x04 && !edit_handle_ctrl_d(history, state))
			break ;
		else if (cbuf[0] == 0x1b)
			if (read(STDIN_FILENO, cbuf, 1) == 1 && cbuf[0] == '[')
				edit_handle_escape_sequence (history, state);
	}
	splay_release(rope);
	return (NULL);
}

int	rope_getc(t_parse_buffer *buf)
{
	t_rope			*rope;
	unsigned char	ch;

	rope = NULL;
	if (buf->cur_pos == buf->size)
		return (EOF);
	splay_assign(&rope, buf->data);
	ch = rope_index(rope, buf->cur_pos++);
	splay_release(rope);
	return (ch);
}

void	edit_init_parse_buffer_with_rope(t_parse_buffer *buf, t_rope *rope)
{
	buf->cur_pos = 0;
	buf->size = rope_length(rope);
	buf->lex_stat = LEXSTAT_NORMAL;
	buf->getc = rope_getc;
	buf->ungetc = NULL;
	buf->data = rope;
}

int	edit_read_execute(t_command_history *history, t_command_state *state)
{
	t_rope				*rope;
	t_token				tok;
	t_parse_ast			*cmdline;
	t_parse_ast			*seqcmd;
	t_parse_buffer		buf;

	set_shell_sighandlers();
	write(STDOUT_FILENO, MINISHELL_PROMPT, MINISHELL_PROMPT_LEN);
	splay_init(&rope, edit_get_line(history, state));
	if (!rope)
		return (1);
	splay_assign(&rope, rope_concat(rope, rope_create("\n", NULL)));
	edit_init_parse_buffer_with_rope(&buf, rope);
	lex_get_token(&buf, &tok);
	cmdline = parse_command_line(&buf, &tok);
	splay_release(rope);
	if (!cmdline)
	{
		put_err_msg("Parse error.");
		set_status(1);
		return (1);
	}
	seqcmd = cmdline->content.command_line->seqcmd_node;
	tty_reset(STDIN_FILENO);
	set_sighandlers_during_execution();
	invoke_sequential_commands(seqcmd);
	if (g_shell.signal_child_received)
	{
		if (g_shell.signal_child_received == SIGQUIT)
			write(STDOUT_FILENO, "Quit (core dumped)", 18);
		write(STDOUT_FILENO, "\n", 1);
		set_status(128 + g_shell.signal_child_received);
	}
	set_shell_sighandlers();
	if (tty_cbreak(STDIN_FILENO) < 0)
		edit_error_exit("tty_cbreak error");
	parse_free_all_ast();
	return (1);
}

int	edit_main(void)
{
	t_command_history	history;
	t_command_state		state;

	edit_init_history(&history);
	edit_setup_terminal();
	state.cursor_x = 0;
	state.length = 0;
	edit_term_controls_init(&state.cnt);
	g_shell.running = 1;
	while (g_shell.running)
		edit_read_execute(&history, &state);
	edit_cleanup_history(&history);
	if (tty_reset(STDIN_FILENO) < 0)
		edit_error_exit("tty_reset error");
	return (0);
}
