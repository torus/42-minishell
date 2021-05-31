#ifndef EDITOR_H
# define EDITOR_H

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
	char	areabuf[128];
	char	*c_cursor_left;
	char	*c_cursor_right;
	char	*c_cursor_up;
	char	*c_cursor_down;
	char	*c_clr_bol;
	char	*c_enter_insert_mode;
	char	*c_exit_insert_mode;
	char	*c_delete_character;
	char	*c_cursor_address;
	char	*c_save_cursor;
	char	*c_restore_cursor;
}	t_term_controls;

typedef struct s_command_state
{
	int				cursor_x;
	int				length;
	t_term_controls	cnt;
}	t_command_state;

int		edit_main(void);
void	edit_terminal_state_init(t_terminal_state *st);
int		edit_putc(int ch);
int		tty_reset(int fd);
void	edit_error_exit(const char *message);
int		tty_set_attributes(int fd, struct termios *buf);
int		tty_cbreak(int fd);
void	edit_init_history(t_command_history *his);
int		edit_print_history(t_command_history *his, int his_index, int index);
void	edit_dump_history(t_command_history *his);
void	edit_add_new_rope(t_command_history *history, char *cbuf);
void	edit_insert_character(
			t_command_history *history, char *cbuf,
			int cursor_x, int command_length);
void	edit_normal_character(
			t_command_history *history, t_command_state *st,
			char *cbuf);
void	edit_enter(t_command_history *history, t_command_state *st);
void	edit_sig_catch(int signo);
int		edit_handle_left(t_command_state *st, char c);
int		edit_handle_right(t_command_state *st, char c);
int		edit_handle_up_down(
			t_command_history *history, t_command_state *st, char c);
void	edit_handle_escape_sequence(
			t_command_history *history, t_command_state *st);
void	edit_term_controls_init(t_term_controls *t);
int		edit_setup_terminal(void);

int		edit_handle_delete(
			t_command_history *history, t_command_state *st, char ch);
void	edit_handle_backspace(t_command_history *history, t_command_state *st);

#endif
