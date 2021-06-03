#include "editor.h"
#include "minishell.h"

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
