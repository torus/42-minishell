#ifndef INPUT_H
# define INPUT_H

# include <term.h>

# include "rope.h"

# define LINE_BUFFER_SIZE 8

typedef enum e_tty_state
{
	TTY_RESET,
	TTY_CBREAK
}	t_tty_state;

typedef struct s_terminal_state
{
	struct termios	save_termios;
	t_tty_state		ttystate;
}	t_terminal_state;

extern t_terminal_state	g_term_stat;

typedef struct s_command_history
{
	t_rope	*ropes[LINE_BUFFER_SIZE];
	int		begin;
	int		end;
	int		current;
}	t_command_history;

typedef struct s_term_controls
{
	char	areabuf[32];
	char	*c_cursor_left;
	char	*c_cursor_right;
	char	*c_clr_bol;
	char	*c_enter_insert_mode;
	char	*c_exit_insert_mode;
}	t_term_controls;

typedef struct s_command_state
{
	int				cursor_x;
	int				length;
	t_term_controls	cnt;
}	t_command_state;

int	edit_main(void);

#endif
