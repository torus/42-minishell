#include "editor.h"
#include "minishell.h"

static int	edit_read_char(
		t_command_history *history, t_command_state *state, char *cbuf)
{
	int	input_count;

	input_count = 0;
	while (input_count == 0)
	{
		input_count = read(STDIN_FILENO, cbuf, 1);
		if (g_shell.interrupted)
		{
			g_shell.interrupted = 0;
			splay_assign(&history->ropes[history->current], NULL);
			state->length = 0;
			state->cursor_x = 0;
			edit_putc('\n');
			input_count = -1;
		}
	}
	return (input_count);
}

t_rope	*edit_get_line(t_command_history *history, t_command_state *state)
{
	char	cbuf[2];
	t_rope	*rope;

	cbuf[1] = '\0';
	while (1)
	{
		if (edit_read_char(history, state, cbuf) != 1
			|| (cbuf[0] == 0x04 && !edit_handle_ctrl_d(history, state)))
			break ;
		if (cbuf[0] == '\n')
		{
			splay_init(&rope, history->ropes[history->current]);
			edit_enter(history, state);
			if (rope)
				rope->refcount--;
			return (rope);
		}
		if (cbuf[0] >= 0x20)
			edit_normal_character(history, state, cbuf);
		else if (cbuf[0] == 0x1b)
			if (read(STDIN_FILENO, cbuf, 1) == 1 && cbuf[0] == '[')
				edit_handle_escape_sequence (history, state);
	}
	return (NULL);
}

int	edit_handle_ctrl_d(t_command_history *history, t_command_state *st)
{
	t_rope	*rope;

	splay_init(&rope, history->ropes[history->current]);
	if (rope_length(rope) == 0)
	{
		g_shell.running = 0;
		write(1, "exit\n", 5);
	}
	else
		edit_delete_char(history, st);
	splay_release(rope);
	return (g_shell.running);
}

void	edit_cleanup_history(t_command_history *history)
{
	int	i;

	i = 0;
	while (i < LINE_BUFFER_SIZE)
		splay_assign(&history->ropes[i++], NULL);
}
