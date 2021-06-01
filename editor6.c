#include <unistd.h>
#include "minishell.h"
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
	edit_redraw(history, st);
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
	edit_handle_left(st, 'D');
	delete_char(history, st);
}

/*
 * (The escape sequence for the left key is '^[[D'.)
 */
int	edit_handle_left(t_command_state *st, char c)
{
	int	col;

	if (c == 'D')
	{
		if (st->cursor_x > 0)
		{
			if ((st->cursor_x + MINISHELL_PROMPT_LEN) % tgetnum("co") > 0)
				tputs(st->cnt.c_cursor_left, 1, edit_putc);
			else
			{
				tputs(st->cnt.c_cursor_up, 1, edit_putc);
				col = tgetnum("co");
				while (col-- > 1)
					tputs(st->cnt.c_cursor_right, 1, edit_putc);
			}
			st->cursor_x--;
		}
		return (1);
	}
	return (0);
}

int	edit_handle_right(t_command_state *st, char c)
{
	int	col;

	if (c == 'C')
	{
		if (st->cursor_x < st->length)
		{
			col = tgetnum("co");
			if ((st->cursor_x + MINISHELL_PROMPT_LEN) % tgetnum("co") < col - 1)
				tputs(st->cnt.c_cursor_right, 1, edit_putc);
			else
			{
				tputs(st->cnt.c_cursor_down, 1, edit_putc);
				while (col-- > 1)
					tputs(st->cnt.c_cursor_left, 1, edit_putc);
			}
			st->cursor_x++;
		}
		return (1);
	}
	return (0);
}
