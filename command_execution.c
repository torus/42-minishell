#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "libft/libft.h"

#define MAXLINE 1000

static void sig_int(int signo);  /* Ctrl + C */
static void sig_quit(int signo); /* Ctrl + \ */

int	main(int argc, char **argv)
{
	char	*line;
	char	**line_args;
	pid_t	pid;
	int		status;
	int		gnl_status;

	if ((signal(SIGINT, sig_int)) == SIG_ERR)
		printf("signal(SIGINT) error\n");
	if ((signal(SIGQUIT, sig_quit)) == SIG_ERR)
		printf("signal(SIGQUIT) error\n");
	ft_putstr_fd("$ ", STDOUT_FILENO);  // プロンプトを表示
	while ((gnl_status = get_next_line(STDIN_FILENO, &line)) == SUCCESS)
	{
		if ((pid = fork()) < 0){
			printf("fork() error\n");
		}else if (pid == 0){  /* 子プロセスの処理 */
			line_args = ft_split(line, ' ');
			if (!line_args)
			{
				printf("ft_split() error. input: |%s|\n", line);
				exit(127);
			}
			/*
			for (int i = 0; line_args[i]; i++)
				printf("line_args[%d]: |%s|\n", i, line_args[i]);
				*/
			execvp(line_args[0], line_args);
			printf("couldn't execute %s\n", line);
			exit(127);
		}
		free(line);

		/* 親プロセスの処理 */
		if ((pid = waitpid(pid, &status, 0)) < 0)
		{
			printf("waitpid() error\n");
		}
		ft_putstr_fd("$ ", STDOUT_FILENO);  // プロンプトを表示
	}
	free(line);
}

void	sig_int(int signo)
{
	printf("receive SIGINT: %d\n", signo);
	exit(0);
}

void	sig_quit(int signo)
{
	printf("receive SIGQUIT: %d\n", signo);
	exit(0);
}
