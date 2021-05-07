#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "libft/libft.h"

#ifndef PATH_H
# define PATH_H

extern char	*g_cwd;
char	*path_join(char *dirpath, char *filename);
int		set_current_working_directory(char *abs_path);
char	*get_abs_path_from_cwd(char *relative_path);
char	*canonicalize_path(char *path);
char	*change_to_parent_dir(char *path);

#endif
