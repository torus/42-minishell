#include <termios.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>

#include <stdio.h>
#include <string.h>

#include "rope.h"

static struct termios		save_termios;
static int					ttysavefd = -1;
static enum { RESET, RAW, CBREAK }	ttystate = RESET;

int	tty_reset(int fd)		/* restore terminal's mode */
{
	if (ttystate == RESET)
		return(0);
	if (tcsetattr(fd, TCSAFLUSH, &save_termios) < 0)
		return(-1);
	ttystate = RESET;
	return(0);
}

static void	sig_catch(int signo)
{
	printf("signal caught %d\n", signo);
	tty_reset(STDIN_FILENO);
	exit(0);
}

#define	MAXLINE	4096			/* max line length */

/*
 * Print a message and return to caller.
 * Caller specifies "errnoflag".
 */
static void	err_doit(int errnoflag, int error, const char *fmt, va_list ap)
{
	char	buf[MAXLINE];

	vsnprintf(buf, MAXLINE-1, fmt, ap);
	if (errnoflag)
		snprintf(buf+strlen(buf), MAXLINE-strlen(buf)-1, ": %s",
		  strerror(error));
	strcat(buf, "\n");
	fflush(stdout);		/* in case stdout and stderr are the same */
	fputs(buf, stderr);
	fflush(NULL);		/* flushes all stdio output streams */
}

/*
 * Fatal error related to a system call.
 * Print a message and terminate.
 */
void
err_sys(const char *fmt, ...)
{
	va_list		ap;

	va_start(ap, fmt);
	err_doit(1, errno, fmt, ap);
	va_end(ap);
	exit(1);
}

int	tty_cbreak(int fd)
{
	int				err;
	struct termios	buf;

	if (ttystate != RESET) {
		errno = EINVAL;
		return(-1);
	}
	if (tcgetattr(fd, &buf) < 0)
		return(-1);
	save_termios = buf;	/* structure copy */

	buf.c_lflag &= ~(ECHO | ICANON);

	buf.c_cc[VMIN] = 1;
	buf.c_cc[VTIME] = 0;
	if (tcsetattr(fd, TCSAFLUSH, &buf) < 0)
		return(-1);

	if (tcgetattr(fd, &buf) < 0) {
		err = errno;
		tcsetattr(fd, TCSAFLUSH, &save_termios);
		errno = err;
		return(-1);
	}
	if ((buf.c_lflag & (ECHO | ICANON)) || buf.c_cc[VMIN] != 1 ||
	  buf.c_cc[VTIME] != 0) {
		tcsetattr(fd, TCSAFLUSH, &save_termios);
		errno = EINVAL;
		return(-1);
	}

	ttystate = CBREAK;
	ttysavefd = fd;
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

#include <term.h>

int	edit_putc(int ch)
{
    write(1, &ch, 1);
    return (1);
}

void	print_history(t_command_history *his, int index)
{
	t_rope	*rope;
	int	len;
	int	i;
	char	ch;

	rope = his->ropes[index];
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

int	main(void)
{
	int		i;
    unsigned char	c;
    char	cbuf[2];

    cbuf[1] = '\0';

	t_command_history	history;
	init_history(&history);

/*
 * isatty, ttyname, ttyslot, ioctl, getenv, tcsetattr, tcgetattr,
 * tgetent, tgetflag, tgetnum, tgetstr, tgoto, tputs
 */

    const char	*term = getenv("TERM");
    if (!term)
		err_sys("getenv(TERM) error");
    fprintf(stderr, "TERM = '%s'\n", term);
    tgetent(NULL, term);

    char	areabuf[32];
    char	*area;

    area = areabuf;
    /* char	*bl = tgetstr("bl", &area); */
    /* tputs(bl, 1, edit_putc); */
    char	*c_left = tgetstr("le", &area);
    char	*c_right = tgetstr("nd", &area);
    char	*c_clear = tgetstr("cb", &area);

	if (signal(SIGINT, sig_catch) == SIG_ERR)	/* catch signals */
		err_sys("signal(SIGINT) error");
	if (signal(SIGQUIT, sig_catch) == SIG_ERR)
		err_sys("signal(SIGQUIT) error");
	if (signal(SIGTERM, sig_catch) == SIG_ERR)
		err_sys("signal(SIGTERM) error");

	if (tty_cbreak(STDIN_FILENO) < 0)
		err_sys("tty_cbreak error");
	printf("\nEnter cbreak mode characters, terminate with SIGINT\n");

	int	cursor_x;

	cursor_x = 0;


	while (1) {
		i = read(STDIN_FILENO, cbuf, 1);

		if (i != 1)
			break ;

        c = cbuf[0];
		c &= 255;

        if (c >= 0x20 && c < 0x7f)
        {
            edit_putc(c);


            if (!history.ropes[history.current])
            {
                if (history.current == history.end)
                {
                    history.end = (history.end + 1) % LINE_BUFFER_SIZE;
                    if (history.end == history.begin)
                        history.begin = (history.end + 1) % LINE_BUFFER_SIZE;
                }
                history.ropes[history.current] = rope_create(cbuf, NULL);
            }
            else
            {
                t_rope	*new_rope;
                new_rope = rope_create(cbuf, NULL);
                history.ropes[history.current] =
                    rope_concat(history.ropes[history.current], new_rope);
            }

        }
        else if (c == '\n')
		{
			history.current = history.end;
			history.ropes[history.current] = NULL;
            edit_putc(c);
		}
        else if (c == 0x1b)
        {
            i = read(STDIN_FILENO, cbuf, 1);
			c = cbuf[0];
            if (c == '[')
            {
                i = read(STDIN_FILENO, cbuf, 1);
				if (i != 1)
					break ;
                c = cbuf[0];
                if (c == 'D')
				{
                    tputs(c_left, 1, edit_putc);
					cursor_x--;
					if (cursor_x < 0)
						cursor_x = 0;
				}
                else if (c == 'C')
				{
                    tputs(c_right, 1, edit_putc);
					cursor_x++;
				}
                else if (c == 'B')
				{
					/* DOWN */
					if (history.current != history.end)
					{
						history.current = (history.current + 1) % LINE_BUFFER_SIZE;
						tputs(c_clear, 1, edit_putc);
						edit_putc('\r');
						print_history(&history, history.current);
					}
				}
                else if (c == 'A')
				{
					/* UP */
					if (history.current != history.begin)
					{
						history.current = (LINE_BUFFER_SIZE + history.current - 1) % LINE_BUFFER_SIZE;
						tputs(c_clear, 1, edit_putc);
						edit_putc('\r');
						print_history(&history, history.current);
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
							dump_history(&history);
                        }
                    }
                }
                else
                    /* printf("\\x%02x", c); */
                    edit_putc(c);
            }
        }
        else
		{
            printf("\\x%02x", c);
		}
        fflush(stdout);
	}
	if (tty_reset(STDIN_FILENO) < 0)
		err_sys("tty_reset error");
	if (i <= 0)
		err_sys("read error");

	exit(0);
}
