#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include "rope.h"
#include "editor.h"
#include "parse.h"
#include "minishell.h"

int	edit_handle_up_down(t_command_history *history, t_command_state *st, char c)
{
	if (c != 'A' && c != 'B')
		return (0);
	if (c == 'B' && history->current != st->current_history_index)
	{
		history->current = (history->current + 1) % LINE_BUFFER_SIZE;
		tputs(st->cnt.c_clr_bol, 1, edit_putc);
		edit_putc('\r');
		write(STDOUT_FILENO, MINISHELL_PROMPT, MINISHELL_PROMPT_LEN);
		st->cursor_x = edit_print_history(history, history->current, 0);
		st->length = st->cursor_x;
	}
	else if (c == 'A' && history->current != history->begin)
	{
		history->current = (LINE_BUFFER_SIZE + history->current - 1)
			% LINE_BUFFER_SIZE;
		tputs(st->cnt.c_clr_bol, 1, edit_putc);
		edit_putc('\r');
		write(STDOUT_FILENO, MINISHELL_PROMPT, MINISHELL_PROMPT_LEN);
		st->cursor_x = edit_print_history(history, history->current, 0);
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
	if (edit_handle_left(st, cbuf[0])
		|| edit_handle_right(st, cbuf[0])
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
	t->c_cursor_up = tgetstr("up", &area);
	t->c_cursor_down = tgetstr("do", &area);
	t->c_clr_bol = tgetstr("cb", &area);
	t->c_clr_eol = tgetstr("ce", &area);
	t->c_enter_insert_mode = tgetstr("im", &area);
	t->c_exit_insert_mode = tgetstr("ei", &area);
	t->c_delete_character = tgetstr("dc", &area);
	t->c_save_cursor = tgetstr("sc", &area);
	t->c_restore_cursor = tgetstr("rc", &area);
}

int	edit_setup_terminal(void)
{
	const char	*term = getenv("TERM");

	if (!term)
		edit_error_exit("getenv(TERM) error");
	tgetent(NULL, term);
	if (tty_cbreak(STDIN_FILENO) < 0)
		edit_error_exit("tty_cbreak error");
	return (1);
}
