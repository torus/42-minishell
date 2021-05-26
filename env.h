#ifndef ENV_H
# define ENV_H

# include <stdbool.h>

// 環境変数とシェル変数をリストとして保持する
typedef struct	s_var {
	char				*key;
	char				*value;
	bool				is_shell_var;
	struct s_var		*next;
}				t_var;

void	free_vars(t_var *vars);
t_var	*add_new_var(t_var **vars, t_var *new_var);
t_var	*kvstr2t_var(char *kvstr, bool is_shell_var);
t_var	*environ2t_var(char **environ);
char	**t_var2environ(t_var *vars);

char	*get_env(const char *env_key);
char	**split_first_c(const char *str, char c);
char	**get_colon_units(char *str, char *default_str);
char	*get_val_from_kvstr(const char *kvstr, char delimiter);
char	*get_env_val(char *env_key);
char	*expand_env_var(char *str);
char	*generate_kvstr(const char *key, const char *value);
int		ft_setenv(const char *name, const char *value, int rewrite);
int		ft_unsetenv(const char *name);

// コマンドの終了ステータス
int		get_status(void);
void	set_status(int status_value);
int		set_status_and_ret(int status_value, int ret_value);

#endif
