#ifndef ENV_H
# define ENV_H

# include <unistd.h>
# include <stdlib.h>
# include "libft/libft.h"

char	*get_env(const char *env_key);
char	**split_first_c(const char *str, char c);
char	*get_val_from_kvstr(const char *kvstr, char delimiter);
char	*get_env_val(char *env_key);
char	*expand_env_var(char *str);
int		setenv(const char *name, const char *value, int rewrite);
int		unsetenv(const char *name);

// コマンドの終了ステータス
int		get_status(void);
void	set_status(int status_value);

#endif
