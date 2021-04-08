#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	DIR *dp;
	struct dirent *dirp;
	struct stat buf;

	if (argc != 2)
		return (1);
	if ((dp = opendir(argv[1])) == NULL)
		return (1);
	while ((dirp = readdir(dp)) != NULL)
	{
		// display filename
		printf("%s\n", dirp->d_name);
		// get file status
		if (stat(dirp->d_name, &buf) == -1)
		{
			printf("\terror: stat()\n");
			continue;
		}
		if (S_ISREG(buf.st_mode))
			printf("\ttype: Regular\n");
		else if (S_ISDIR(buf.st_mode))
			printf("\ttype: Directory\n");
		else if (S_ISLNK(buf.st_mode))
			printf("\ttype: Link\n");
		else
			printf("\ttype: Other\n");
	}
	closedir(dp);
	return (0);
}

