#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "libft/libft.h"

#ifndef PATH_H
# define PATH_H

extern char	*g_cwd;
char	*path_join(char *dirpath, char *filename);
int		set_cwd(char *abs_path);
char	*get_cwd_path(char *caller);
int		is_cwd_valid(char *for_whom);
char	*get_abs_path_from_cwd(char *relative_path);
char	*canonicalize_path(char *path);
char	*change_to_parent_dir(char *path);
bool	is_directory(char *path);
void	put_cwd_err_msg(char *for_whom);

#endif
