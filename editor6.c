#include <unistd.h>
#include "editor.h"

int	edit_handle_delete(
		t_command_history *history, t_command_state *st, char ch)
{
	t_rope	*rope;
	int		i;

	if (ch != '3')
		return (0);
	else
	{
		i = read(STDIN_FILENO, &ch, 1);
		if (i != 1 || ch != '~')
		{
			return (0);
		}
	}
	splay_init(&rope, history->ropes[history->current]);
	if (!rope)
		return (1);
	if (st->cursor_x < rope_length(rope))
	{
		tputs(st->cnt.c_delete_character, 1, edit_putc);
		splay_assign(
			&history->ropes[history->current],
			rope_delete(rope, st->cursor_x, st->cursor_x + 1));
	}
	splay_release(rope);
	return (1);
}

void	edit_handle_backspace(t_command_history *history, t_command_state *st)
{
	(void)history;
	(void)st;
}
