#ifndef EXECUTION_H
# define EXECUTION_H

# include <unistd.h>
# include <stdio.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <signal.h>
# include "libft/libft.h"

# define MAXLINE 1000

/* Signal handlers */
static void	sig_int(int signo);  /* Ctrl + C */
static void	sig_quit(int signo); /* Ctrl + \ */

/* Execution */
char	*find_executable_file(char *filename);
int	ft_execvp(char *filename, char **argv);

#endif

