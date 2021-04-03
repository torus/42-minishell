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
			execlp(line, "", (char *)0);
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
