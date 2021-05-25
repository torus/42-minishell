#include <stdio.h>
#include "editor.h"

void	edit_dump_history(t_command_history *his)
{
	int	index;

	printf("\n**** HISTORY: beg: %d, cur: %d, end: %d\n",
		his->begin, his->current, his->end);
	index = his->begin;
	while (index != his->end)
	{
		printf("| '");
		edit_print_history(his, index);
		printf("'\n");
		index = (index + 1) % LINE_BUFFER_SIZE;
	}
}

void	edit_add_new_rope(t_command_history *history, char *cbuf)
{
	if (history->current == history->end)
	{
		history->end = (history->end + 1) % LINE_BUFFER_SIZE;
		if (history->end == history->begin)
			history->begin = (history->end + 1) % LINE_BUFFER_SIZE;
	}
	/* splay_release(history->ropes[history->current]); */
	/* history->ropes[history->current] = rope_create(cbuf, NULL); */
	/* history->ropes[history->current]->refcount++; */
	splay_assign(
		&history->ropes[history->current], rope_create(cbuf, NULL));
}

void	edit_insert_character(
			t_command_history *history, char *cbuf,
			int cursor_x, int command_length)
{
	t_rope	*new_rope;
	/* t_rope	*old_rope; */

	new_rope = NULL;
	/* new_rope = rope_create(cbuf, NULL); */
	splay_assign(&new_rope, rope_create(cbuf, NULL));
	/* old_rope = history->ropes[history->current]; */
	/* new_rope->refcount++; */
	if (cursor_x == command_length)
	{
		/* history->ropes[history->current] */
		/* 	= rope_concat(history->ropes[history->current], new_rope); */
		/* history->ropes[history->current]->refcount++; */
		/* splay_release(old_rope); */
		splay_assign(
			&history->ropes[history->current],
			rope_concat(history->ropes[history->current], new_rope));
	}
	else if (cursor_x == 0)
	{
		splay_assign(
			&history->ropes[history->current],
			rope_concat(new_rope, history->ropes[history->current]));
		/* history->ropes[history->current] */
		/* 	= rope_concat(new_rope, history->ropes[history->current]); */
		/* history->ropes[history->current]->refcount++; */
		/* splay_release(old_rope); */
	}
	else if (cursor_x < command_length)
	{
		splay_assign(
			&history->ropes[history->current],
			rope_insert(history->ropes[history->current], cursor_x, new_rope));
		/* history->ropes[history->current] */
		/* 	= rope_insert(history->ropes[history->current], cursor_x, new_rope); */
		/* history->ropes[history->current]->refcount++; */
		/* splay_release(old_rope); */
	}
	splay_release(new_rope);
}

void	edit_normal_character(
	t_command_history *history, t_command_state *st,
	char *cbuf)
{
	edit_putc(cbuf[0]);
	if (!history->ropes[history->current])
		edit_add_new_rope(history, cbuf);
	else
		edit_insert_character(history, cbuf, st->cursor_x, st->length);
	st->cursor_x++;
	st->length++;
}

void	edit_enter(t_command_history *history, t_command_state *st)
{
	history->current = history->end;
	splay_release(history->ropes[history->current]);
	history->ropes[history->current] = NULL;
	st->cursor_x = 0;
	st->length = 0;
	edit_putc('\n');
}
