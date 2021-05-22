#ifndef ENV_H
# define ENV_H

char	*get_env(const char *env_key);
char	**split_first_c(const char *str, char c);
char	**get_colon_units(char *str, char *default_str);
char	*get_val_from_kvstr(const char *kvstr, char delimiter);
char	*get_env_val(char *env_key);
char	*expand_env_var(char *str);
int		ft_setenv(const char *name, const char *value, int rewrite);
int		ft_unsetenv(const char *name);

// コマンドの終了ステータス
int		get_status(void);
void	set_status(int status_value);
int		set_status_and_ret(int status_value, int ret_value);

#endif
