#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "editor.h"

t_terminal_state	g_term_stat;

void	edit_terminal_state_init(t_terminal_state *st)
{
	st->ttystate = TTY_RESET;
}

int	edit_putc(int ch)
{
	write(1, &ch, 1);
	return (1);
}

int	tty_reset(int fd)
{
	if (g_term_stat.ttystate == TTY_RESET)
		return (0);
	if (tcsetattr(fd, TCSAFLUSH, &g_term_stat.save_termios) < 0)
		return (-1);
	g_term_stat.ttystate = TTY_RESET;
	return (0);
}

void	edit_sig_catch(int signo)
{
	printf("signal caught %d\n", signo);
	tty_reset(STDIN_FILENO);
	exit(0);
}

void	edit_redraw(t_command_history *history, t_command_state *st)
{
	tputs(st->cnt.c_save_cursor, 1, edit_putc);
	edit_print_history(history, history->current, st->cursor_x);
	tputs(st->cnt.c_clr_eol, 1, edit_putc);
	tputs(st->cnt.c_restore_cursor, 1, edit_putc);
}
