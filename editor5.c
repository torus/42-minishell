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
	char	areabuf[32];
	char	*area;
	char	*c_exit_insert_mode;

	area = areabuf;
	c_exit_insert_mode = tgetstr("ei", &area);
	if (g_term_stat.ttystate == TTY_RESET)
		return (0);
	if (tcsetattr(fd, TCSAFLUSH, &g_term_stat.save_termios) < 0)
		return (-1);
	g_term_stat.ttystate = TTY_RESET;
	tputs(c_exit_insert_mode, 1, edit_putc);
	return (0);
}

void	edit_sig_catch(int signo)
{
	printf("signal caught %d\n", signo);
	tty_reset(STDIN_FILENO);
	exit(0);
}
