#ifndef PATH_H
# define PATH_H

# include "libft/libft.h"

extern char	*g_cwd;
int			set_cwd(char *abs_path);
char		*get_cwd_path(char *caller);
int			is_cwd_valid(char *for_whom);
char		*get_abs_path_from_cwd(char *relative_path);
void		put_cwd_err_msg(char *for_whom);
const char	*canonicalize_path(const char *path);
bool		is_directory(const char *path);
const char	*path_join(const char *dirpath, const char *filename);
const char	*get_parent_dir(const char *path);
bool		is_executable(const char *path);

#endif
