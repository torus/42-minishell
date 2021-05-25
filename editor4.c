#include <unistd.h>
#include <errno.h>
#include <term.h>
#include <string.h>
#include "libft/libft.h"
#include "editor.h"

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