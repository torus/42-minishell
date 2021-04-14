#ifndef ENV_H
# define ENV_H

# include <unistd.h>
# include <stdlib.h>
# include "libft/libft.h"

char	*get_env(const char *env_key);
char	**split_first_c(const char *str, char c);
char	*get_val_from_kvstr(const char *kvstr, char key);
char	*get_env_val(char *env_key);

#endif