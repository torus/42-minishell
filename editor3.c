#include <stdio.h>
#include "minishell.h"
#include "editor.h"

void	edit_dump_history(t_command_history *his)
{
	int	index;

	printf("\n**** HISTORY: beg: %d, cur: %d, end: %d\n",
		his->begin, his->current, his->end);
	index = his->begin;
	while (index != his->end)
	{
		write(1, "| '", 3);
		edit_print_history(his, index, 0);
		write(1, "'\n", 2);
		index = (index + 1) % LINE_BUFFER_SIZE;
	}
}

void	edit_add_new_rope(t_command_history *history, char *cbuf)
{
	edit_adjust_history_index(history);
	splay_assign(
		&history->ropes[history->current], rope_create(cbuf, NULL));
}

void	edit_insert_character(
			t_command_history *history, char *cbuf,
			int cursor_x, int command_length)
{
	t_rope	*new_rope;

	splay_init(&new_rope, rope_create(cbuf, NULL));
	if (cursor_x == command_length)
	{
		splay_assign(
			&history->ropes[history->current],
			rope_concat(history->ropes[history->current], new_rope));
	}
	else if (cursor_x == 0)
	{
		splay_assign(
			&history->ropes[history->current],
			rope_concat(new_rope, history->ropes[history->current]));
	}
	else if (cursor_x < command_length)
	{
		splay_assign(
			&history->ropes[history->current],
			rope_insert(history->ropes[history->current], cursor_x, new_rope));
	}
	splay_release(new_rope);
}

void	edit_normal_character(
	t_command_history *history, t_command_state *st,
	char *cbuf)
{
	int	col;

	edit_copy_history_if_needed(history, st);
	col = tgetnum("co");
	if (cbuf[0] == 0x7f)
	{
		edit_handle_backspace(history, st);
		return ;
	}
	edit_putc(cbuf[0]);
	if (!history->ropes[history->current])
		edit_add_new_rope(history, cbuf);
	else
		edit_insert_character(history, cbuf, st->cursor_x, st->length);
	st->cursor_x++;
	st->length++;
	if ((st->cursor_x + MINISHELL_PROMPT_LEN) % col == 0)
	{
		tputs(st->cnt.c_cursor_down, 1, edit_putc);
		write(1, "\r", 1);
	}
	edit_redraw(history, st);
}

/*
 * This function is called when the Enter key is pressed to execute
 * the given command.  Calling edit_print_history() to make sure the
 * cursor is on the end of the line before the execution otherwise the
 * output from the command may overwrite the current command line.
 */
void	edit_enter(t_command_history *history, t_command_state *st)
{
	edit_print_history(history, history->current, st->cursor_x);
	history->current = history->end;
	splay_release(history->ropes[history->current]);
	history->ropes[history->current] = NULL;
	st->cursor_x = 0;
	st->length = 0;
	edit_putc('\n');
}
