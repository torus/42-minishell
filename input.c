#include <termios.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <stdio.h>
#include <term.h>

#include "libft/libft.h"
#include "rope.h"

typedef struct s_terminal_state
{
	struct termios		save_termios;
	enum
	{
		TTY_RESET,
		TTY_CBREAK
	}					ttystate;
}	t_terminal_state;

t_terminal_state	g_term_stat;

void	terminal_state_init(t_terminal_state *st)
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
		return(0);
	if (tcsetattr(fd, TCSAFLUSH, &g_term_stat.save_termios) < 0)
		return(-1);
	g_term_stat.ttystate = TTY_RESET;
	tputs(c_exit_insert_mode, 1, edit_putc);
	return(0);
}

static void	sig_catch(int signo)
{
	printf("signal caught %d\n", signo);
	tty_reset(STDIN_FILENO);
	exit(0);
}

void	error_exit(const char *message)
{
	write(STDERR_FILENO, message, ft_strlen(message));
	exit (1);
}

int	tty_cbreak(int fd)
{
	int				err;
	struct termios	buf;

	if (g_term_stat.ttystate != TTY_RESET) {
		errno = EINVAL;
		return(-1);
	}
	if (tcgetattr(fd, &buf) < 0)
		return(-1);
	g_term_stat.save_termios = buf;	/* structure copy */

	buf.c_lflag &= ~(ECHO | ICANON);

	buf.c_cc[VMIN] = 1;
	buf.c_cc[VTIME] = 0;
	if (tcsetattr(fd, TCSAFLUSH, &buf) < 0)
		return(-1);

	if (tcgetattr(fd, &buf) < 0) {
		err = errno;
		tcsetattr(fd, TCSAFLUSH, &g_term_stat.save_termios);
		errno = err;
		return(-1);
	}
	if ((buf.c_lflag & (ECHO | ICANON)) || buf.c_cc[VMIN] != 1 ||
	  buf.c_cc[VTIME] != 0) {
		tcsetattr(fd, TCSAFLUSH, &g_term_stat.save_termios);
		errno = EINVAL;
		return(-1);
	}

	g_term_stat.ttystate = TTY_CBREAK;
	return(0);
}

#define LINE_BUFFER_SIZE 8

typedef struct	s_command_history
{
	t_rope	*ropes[LINE_BUFFER_SIZE];
	int		begin;
	int		end;
	int		current;
}	t_command_history;

void	init_history(t_command_history *his)
{
	int	i;

	his->begin = 0;
	his->end = 0;
	his->current = 0;
	i = 0;
	while (i < LINE_BUFFER_SIZE)
		his->ropes[i++] = NULL;
}

int	print_history(t_command_history *his, int index)
{
	t_rope	*rope;
	int	len;
	int	i;
	char	ch;

	rope = his->ropes[index];
	len = 0;
	if (rope)
	{
		len = rope_length(rope);
		i = 0;
		while (i < len)
		{
			ch = rope_index(rope, i);
			write(1, &ch, 1);
			i++;
		}
	}
	return (len);
}

void	dump_history(t_command_history *his)
{
	int	index;

	fprintf(stderr, "\n**** HISTORY: beg: %d, cur: %d, end: %d\n",
			his->begin, his->current, his->end);

	index = his->begin;
	while (index != his->end)
	{
		fprintf(stderr, "| '");
		print_history(his, index);
		fprintf(stderr, "'\n");
		index = (index + 1) % LINE_BUFFER_SIZE;
	}
}

void	add_new_rope(t_command_history *history, char *cbuf)
{
	if (history->current == history->end)
	{
		history->end = (history->end + 1) % LINE_BUFFER_SIZE;
		if (history->end == history->begin)
			history->begin = (history->end + 1) % LINE_BUFFER_SIZE;
	}
	history->ropes[history->current] = rope_create(cbuf, NULL);
}

void	edit_insert_character(
			t_command_history *history, char *cbuf,
			int cursor_x, int command_length)
{
	t_rope	*new_rope;
	new_rope = rope_create(cbuf, NULL);
	if (cursor_x == command_length)
		history->ropes[history->current] =
			rope_concat(history->ropes[history->current], new_rope);
	else if (cursor_x == 0)
	{
		history->ropes[history->current] =
			rope_concat(new_rope, history->ropes[history->current]);
	}
	else if (cursor_x < command_length)
	{
		history->ropes[history->current] =
			rope_insert(history->ropes[history->current], cursor_x, new_rope);
	}
}

typedef struct s_term_controls
{
	char	areabuf[32];
	char	*c_cursor_left;
	char	*c_cursor_right;
	char	*c_clr_bol;
	char	*c_enter_insert_mode;
	char	*c_exit_insert_mode;
}	t_term_controls;

typedef struct	s_command_state
{
	int				cursor_x;
	int				length;
	t_term_controls	cnt;
}	t_command_state;

void	edit_normal_character(
	t_command_history *history, t_command_state *st,
	char *cbuf)
{
	edit_putc(cbuf[0]);
	if (!history->ropes[history->current])
		add_new_rope(history, cbuf);
	else
		edit_insert_character(history, cbuf, st->cursor_x, st->length);
	st->cursor_x++;
	st->length++;
}

void	edit_enter(t_command_history *history, t_command_state *st)
{
	history->current = history->end;
	history->ropes[history->current] = NULL;
	st->cursor_x = 0;
	st->length = 0;
	edit_putc('\n');
}

void	handle_escape_sequence (t_command_history *history, t_command_state *st)
{
	int	i;
	char	c;
	char	cbuf[2];

	i = read(STDIN_FILENO, cbuf, 1);
	if (i != 1)
		return ;
	c = cbuf[0];
	if (c == 'D')
	{
		tputs(st->cnt.c_cursor_left, 1, edit_putc);
		st->cursor_x--;
		if (st->cursor_x < 0)
			st->cursor_x = 0;
	}
	else if (c == 'C')
	{
		if (st->cursor_x < st->length)
		{
			tputs(st->cnt.c_cursor_right, 1, edit_putc);
			st->cursor_x++;
		}
	}
	else if (c == 'B')
	{
		/* DOWN */
		if (history->current != history->end)
		{
			history->current = (history->current + 1) % LINE_BUFFER_SIZE;
			tputs(st->cnt.c_clr_bol, 1, edit_putc);
			edit_putc('\r');
			st->cursor_x = print_history(history, history->current);
			st->length = st->cursor_x;
		}
	}
	else if (c == 'A')
	{
		/* UP */
		if (history->current != history->begin)
		{
			history->current = (LINE_BUFFER_SIZE + history->current - 1) % LINE_BUFFER_SIZE;
			tputs(st->cnt.c_clr_bol, 1, edit_putc);
			edit_putc('\r');
			st->cursor_x = print_history(history, history->current);
			st->length = st->cursor_x;
		}
	}
	else if (c == '1')
	{
		/* F5 */
		i = read(STDIN_FILENO, cbuf, 1);
		if (cbuf[0] == '5')
		{
			i = read(STDIN_FILENO, cbuf, 1);
			if (cbuf[0] == '~')
			{
				dump_history(history);
			}
		}
	}
	else
		/* printf("\\x%02x", c); */
		edit_putc(c);
}

void	term_controls_init(t_term_controls *t)
{
	char	*area;

	area = t->areabuf;
	t->c_cursor_left = tgetstr("le", &area);
	t->c_cursor_right = tgetstr("nd", &area);
	t->c_clr_bol = tgetstr("cb", &area);
	t->c_enter_insert_mode = tgetstr("im", &area);
	t->c_exit_insert_mode = tgetstr("ei", &area);
}

int	setup_terminal(void)
{
	const char	*term = getenv("TERM");
	if (!term)
		error_exit("getenv(TERM) error");
	fprintf(stderr, "TERM = '%s'\n", term);
	tgetent(NULL, term);

	if (signal(SIGINT, sig_catch) == SIG_ERR)
		error_exit("signal(SIGINT) error");
	if (signal(SIGQUIT, sig_catch) == SIG_ERR)
		error_exit("signal(SIGQUIT) error");
	if (signal(SIGTERM, sig_catch) == SIG_ERR)
		error_exit("signal(SIGTERM) error");

	if (tty_cbreak(STDIN_FILENO) < 0)
		error_exit("tty_cbreak error");
	return (1);
}

/*
 * isatty, ttyname, ttyslot, ioctl, getenv, tcsetattr, tcgetattr,
 * tgetent, tgetflag, tgetnum, tgetstr, tgoto, tputs
 */

int	main_loop(t_command_history *history, t_command_state *state)
{
	char				cbuf[2];

	cbuf[1] = '\0';
	while (1) {
		if (read(STDIN_FILENO, cbuf, 1) != 1)
			break ;
		if (cbuf[0] >= 0x20 && cbuf[0] < 0x7f)
			edit_normal_character(history, state, cbuf);
		else if (cbuf[0] == '\n')
			edit_enter(history, state);
		else if (cbuf[0] == 0x1b)
		{
			if (read(STDIN_FILENO, cbuf, 1) == 1 && cbuf[0] == '[')
				handle_escape_sequence (history, state);
		}
	}
	return (1);
}

int	main(void)
{
	t_command_history	history;
	t_command_state		state;

	terminal_state_init(&g_term_stat);
	init_history(&history);
	setup_terminal();
	state.cursor_x = 0;
	state.length = 0;
	term_controls_init(&state.cnt);
	tputs(state.cnt.c_enter_insert_mode, 1, edit_putc);
	main_loop(&history, &state);
	if (tty_reset(STDIN_FILENO) < 0)
		error_exit("tty_reset error");
	return (0);
}
