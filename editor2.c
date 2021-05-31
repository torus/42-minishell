#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include "rope.h"
#include "editor.h"
#include "parse.h"

int	edit_handle_left_right(t_command_state *st, char c)
{
	if (c == 'D')
	{
		if (st->cursor_x > 0)
		{
			tputs(st->cnt.c_cursor_left, 1, edit_putc);
			st->cursor_x--;
		}
		return (1);
	}
	else if (c == 'C')
	{
		if (st->cursor_x < st->length)
		{
			tputs(st->cnt.c_cursor_right, 1, edit_putc);
			st->cursor_x++;
		}
		return (1);
	}
	return (0);
}

int	edit_handle_up_down(t_command_history *history, t_command_state *st, char c)
{
	if (c != 'A' && c != 'B')
		return (0);
	if (c == 'B')
	{
		if (history->current != history->end)
		{
			history->current = (history->current + 1) % LINE_BUFFER_SIZE;
			tputs(st->cnt.c_clr_bol, 1, edit_putc);
			edit_putc('\r');
			st->cursor_x = edit_print_history(history, history->current);
			st->length = st->cursor_x;
		}
		return (1);
	}
	if (history->current != history->begin)
	{
		history->current = (LINE_BUFFER_SIZE + history->current - 1)
			% LINE_BUFFER_SIZE;
		tputs(st->cnt.c_clr_bol, 1, edit_putc);
		edit_putc('\r');
		st->cursor_x = edit_print_history(history, history->current);
		st->length = st->cursor_x;
	}
	return (1);
}

/*
 * '\x1b[5~' => F5 key for debugging
 */

void	edit_handle_escape_sequence(
	t_command_history *history, t_command_state *st)
{
	int		i;
	char	cbuf[2];

	i = read(STDIN_FILENO, cbuf, 1);
	if (i != 1)
		return ;
	if (edit_handle_left_right(st, cbuf[0])
		|| edit_handle_up_down(history, st, cbuf[0])
		|| edit_handle_delete(history, st, cbuf[0]))
		;
	else if (cbuf[0] == '1')
	{
		i = read(STDIN_FILENO, cbuf, 1);
		if (cbuf[0] == '5')
		{
			i = read(STDIN_FILENO, cbuf, 1);
			if (cbuf[0] == '~')
			{
				edit_dump_history(history);
			}
		}
	}
}

void	edit_term_controls_init(t_term_controls *t)
{
	char	*area;

	area = t->areabuf;
	t->c_cursor_left = tgetstr("le", &area);
	t->c_cursor_right = tgetstr("nd", &area);
	t->c_clr_bol = tgetstr("cb", &area);
	t->c_enter_insert_mode = tgetstr("im", &area);
	t->c_exit_insert_mode = tgetstr("ei", &area);
	t->c_delete_character = tgetstr("dc", &area);
}

int	edit_setup_terminal(void)
{
	const char	*term = getenv("TERM");

	if (!term)
		edit_error_exit("getenv(TERM) error");
	tgetent(NULL, term);
	if (signal(SIGINT, edit_sig_catch) == SIG_ERR)
		edit_error_exit("signal(SIGINT) error");
	if (signal(SIGQUIT, edit_sig_catch) == SIG_ERR)
		edit_error_exit("signal(SIGQUIT) error");
	if (signal(SIGTERM, edit_sig_catch) == SIG_ERR)
		edit_error_exit("signal(SIGTERM) error");
	if (tty_cbreak(STDIN_FILENO) < 0)
		edit_error_exit("tty_cbreak error");
	return (1);
}
