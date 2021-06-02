#include "editor.h"
#include "minishell.h"

int	handle_ctrl_d(t_command_history *history, t_command_state *st)
{
	t_rope	*rope;

	splay_init(&rope, history->ropes[history->current]);
	if (rope_length(rope) == 0)
		g_shell.running = 0;
	else
		edit_handle_delete(history, st, '3');
	splay_release(rope);
	return (g_shell.running);
}
