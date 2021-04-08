#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	DIR *dp;
	struct dirent *dirp;

	if (argc != 2)
		return (1);
	if ((dp = opendir(argv[1])) == NULL)
		return (1);
	while ((dirp = readdir(dp)) != NULL)
	{
		// ファイル名
		printf("filename: %s\n", dirp->d_name);
		// i_node番号
		printf("\ti_node: %lu\n", dirp->d_ino);
	}
	closedir(dp);
	return (0);
}

