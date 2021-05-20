#ifndef PATH_H
# define PATH_H

# include "libft/libft.h"

extern char	*g_cwd;
int		set_cwd(char *abs_path);
char	*get_cwd_path(char *caller);
int		is_cwd_valid(char *for_whom);
char	*get_abs_path_from_cwd(char *relative_path);
void	put_cwd_err_msg(char *for_whom);
char	*canonicalize_path(char *path);
bool	is_directory(char *path);
char	*path_join(char *dirpath, char *filename);
char	*get_parent_dir(char *path);
bool	is_executable(char *path);

#endif
