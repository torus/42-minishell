#include <termios.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <stdio.h>
#include <term.h>
#include <string.h>

#include "libft/libft.h"
#include "rope.h"
#include "input.h"
#include "parse.h"
#include "minishell.h"

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

static void	edit_sig_catch(int signo)
{
	printf("signal caught %d\n", signo);
	tty_reset(STDIN_FILENO);
	exit(0);
}

void	edit_error_exit(const char *message)
{
	char	*error_message;

	write(STDERR_FILENO, message, ft_strlen(message));
	write(STDERR_FILENO, "\n", 1);
	if (errno)
	{
		error_message = strerror(errno);
		write(STDERR_FILENO, error_message, ft_strlen(error_message));
		write(STDERR_FILENO, "\n", 1);
	}
	exit (1);
}

int	tty_set_attributes(int fd, struct termios *buf)
{
	int	err;

	buf->c_lflag &= ~(ECHO | ICANON);
	buf->c_cc[VMIN] = 1;
	buf->c_cc[VTIME] = 0;
	if (tcsetattr(fd, TCSAFLUSH, buf) < 0)
		return (-1);
	if (tcgetattr(fd, buf) < 0)
	{
		err = errno;
		tcsetattr(fd, TCSAFLUSH, &g_term_stat.save_termios);
		errno = err;
		return (-1);
	}
	if ((buf->c_lflag & (ECHO | ICANON)) || buf->c_cc[VMIN] != 1
		|| buf->c_cc[VTIME] != 0)
	{
		tcsetattr(fd, TCSAFLUSH, &g_term_stat.save_termios);
		errno = EINVAL;
		return (-1);
	}
	return (0);
}

int	tty_cbreak(int fd)
{
	int				err;
	struct termios	buf;

	if (g_term_stat.ttystate != TTY_RESET)
	{
		errno = EINVAL;
		return (-1);
	}
	if (tcgetattr(fd, &buf) < 0)
		return (-1);
	g_term_stat.save_termios = buf;
	err = tty_set_attributes(fd, &buf);
	if (err)
		return (err);
	g_term_stat.ttystate = TTY_CBREAK;
	return (0);
}

void	edit_init_history(t_command_history *his)
{
	int	i;

	his->begin = 0;
	his->end = 0;
	his->current = 0;
	i = 0;
	while (i < LINE_BUFFER_SIZE)
		his->ropes[i++] = NULL;
}

int	edit_print_history(t_command_history *his, int index)
{
	t_rope	*rope;
	int		len;
	int		i;
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

void	edit_dump_history(t_command_history *his)
{
	int	index;

	fprintf(stderr, "\n**** HISTORY: beg: %d, cur: %d, end: %d\n",
		his->begin, his->current, his->end);
	index = his->begin;
	while (index != his->end)
	{
		fprintf(stderr, "| '");
		edit_print_history(his, index);
		fprintf(stderr, "'\n");
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
	history->ropes[history->current] = rope_create(cbuf, NULL);
}

void	edit_insert_character(
			t_command_history *history, char *cbuf,
			int cursor_x, int command_length)
{
	t_rope	*new_rope;

	new_rope = rope_create(cbuf, NULL);
	if (cursor_x == command_length)
		history->ropes[history->current]
			= rope_concat(history->ropes[history->current], new_rope);
	else if (cursor_x == 0)
		history->ropes[history->current]
			= rope_concat(new_rope, history->ropes[history->current]);
	else if (cursor_x < command_length)
		history->ropes[history->current]
			= rope_insert(history->ropes[history->current], cursor_x, new_rope);
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
	history->ropes[history->current] = NULL;
	st->cursor_x = 0;
	st->length = 0;
	edit_putc('\n');
}

int	edit_handle_left_right(t_command_state *st, char c)
{
	if (c == 'D')
	{
		tputs(st->cnt.c_cursor_left, 1, edit_putc);
		st->cursor_x--;
		if (st->cursor_x < 0)
			st->cursor_x = 0;
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

void	edit_handle_escape_sequence(t_command_history *history, t_command_state *st)
{
	int		i;
	char	cbuf[2];

	i = read(STDIN_FILENO, cbuf, 1);
	if (i != 1)
		return ;
	if (edit_handle_left_right(st, cbuf[0])
		|| edit_handle_up_down(history, st, cbuf[0]))
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
}

int	edit_setup_terminal(void)
{
	const char	*term = getenv("TERM");

	if (!term)
		edit_error_exit("getenv(TERM) error");
	fprintf(stderr, "TERM = '%s'\n", term);
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

/*
 * Allowed functions:
 *
 * printf, malloc, free, write, open, read, close, fork, wait,
 * waitpid, wait3, wait4, signal, kill, exit, getcwd, chdir, stat,
 * lstat, fstat, execve, dup, dup2, pipe, opendir, readdir, closedir,
 * strerror, errno, isatty, ttyname, ttyslot, ioctl, getenv,
 * tcsetattr, tcgetattr, tgetent, tgetflag, tgetnum, tgetstr, tgoto,
 * tputs
 */

t_rope	*edit_get_line(t_command_history *history, t_command_state *state)
{
	char	cbuf[2];
	t_rope	*rope;

	cbuf[1] = '\0';
	while (1)
	{
		if (read(STDIN_FILENO, cbuf, 1) != 1)
			break ;
		if (cbuf[0] >= 0x20 && cbuf[0] < 0x7f)
			edit_normal_character(history, state, cbuf);
		else if (cbuf[0] == '\n')
		{
			rope = history->ropes[history->current];
			edit_enter(history, state);
			return (rope);
		}
		else if (cbuf[0] == 0x1b)
		{
			if (read(STDIN_FILENO, cbuf, 1) == 1 && cbuf[0] == '[')
				edit_handle_escape_sequence (history, state);
		}
	}
	return (NULL);
}

/*
static void	print_rope(t_rope *rope)
{
	int	len;
	int	i;

	i = 0;
	len = rope_length(rope);
	while (i < len)
	{
		printf("%c", rope_index(rope, i));
		i++;
	}
	printf("\n");
}
*/

int	rope_getc(t_parse_buffer *buf)
{
	t_rope			*rope;
	unsigned char	ch;

	if (buf->cur_pos == buf->size)
		return (EOF);
	rope = buf->data;
	ch = rope_index(rope, buf->cur_pos++);
	return (ch);
}

void	edit_init_parse_buffer_with_rope(t_parse_buffer *buf, t_rope *rope)
{
	buf->cur_pos = 0;
	buf->size = rope_length(rope);
	buf->lex_stat = LEXSTAT_NORMAL;
	buf->getc = rope_getc;
	buf->ungetc = NULL;
	buf->data = rope;
}

int	edit_main(void)
{
	t_command_history	history;
	t_command_state		state;
	t_rope				*rope;
	t_token				tok;
	t_parse_ast			*cmdline;
	t_parse_ast			*seqcmd;

	edit_terminal_state_init(&g_term_stat);
	edit_init_history(&history);
	edit_setup_terminal();
	state.cursor_x = 0;
	state.length = 0;
	edit_term_controls_init(&state.cnt);
	tputs(state.cnt.c_enter_insert_mode, 1, edit_putc);
	while (1)
	{
		t_parse_buffer	buf;
		rope = edit_get_line(&history, &state);
		if (rope)
		{
			rope = rope_concat(rope, rope_create("\n", NULL));
			edit_init_parse_buffer_with_rope(&buf, rope);
			lex_get_token(&buf, &tok);
			cmdline = parse_command_line(&buf, &tok);
			if (cmdline)
			{
				seqcmd = cmdline->content.command_line->seqcmd_node;
				invoke_sequential_commands(seqcmd);
				parse_free_all_ast();
			}

		}
	}
	if (tty_reset(STDIN_FILENO) < 0)
		edit_error_exit("tty_reset error");
	return (0);
}
