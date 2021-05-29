#include <unistd.h>
#include "editor.h"

static void	delete_char(t_command_history *history, t_command_state *st)
{
	t_rope	*rope;

	splay_init(&rope, history->ropes[history->current]);
	tputs(st->cnt.c_delete_character, 1, edit_putc);
	st->length--;
	splay_assign(
		&history->ropes[history->current],
		rope_delete(rope, st->cursor_x, st->cursor_x + 1));
	splay_release(rope);
}

int	edit_handle_delete(
		t_command_history *history, t_command_state *st, char ch)
{
	t_rope	*rope;
	int		num;

	if (ch != '3')
		return (0);
	else
	{
		num = read(STDIN_FILENO, &ch, 1);
		if (num != 1 || ch != '~')
			return (0);
	}
	splay_init(&rope, history->ropes[history->current]);
	if (!rope)
		return (1);
	if (st->cursor_x < rope_length(rope))
		delete_char(history, st);
	splay_release(rope);
	return (1);
}

void	edit_handle_backspace(t_command_history *history, t_command_state *st)
{
	if (st->cursor_x == 0)
		return ;
	tputs(st->cnt.c_cursor_left, 1, edit_putc);
	st->cursor_x--;
	delete_char(history, st);
}
