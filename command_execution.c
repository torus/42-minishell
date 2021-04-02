#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "libft/libft.h"

#define MAXLINE 1000

int	main(int argc, char **argv)
{
	char	*line;
	pid_t	pid;
	int		status;
	int		gnl_status;

	printf("%% ");  // プロンプトを表示
	while ((gnl_status = get_next_line(STDIN_FILENO, &line)) == SUCCESS)
	{
		if ((pid = fork()) < 0){
			printf("fork() error\n");
		}else{  /* 子プロセスの処理 */
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
		printf("%% ");  // プロンプトを表示
	}
	free(line);
}
