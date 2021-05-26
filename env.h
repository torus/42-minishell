#ifndef ENV_H
# define ENV_H

# include <stdbool.h>

// 環境変数とシェル変数をリストとして保持する
typedef struct	s_var {
	const char			*key;
	const char			*value;
	bool				is_shell_var;
	struct s_var		*next;
}				t_var;

void	free_vars(t_var *vars);
t_var	*add_new_var(t_var **vars, t_var *new_var);
t_var	*kvstr2var(char *kvstr, bool is_shell_var);
t_var	*environ2vars(char **environ);
char	**vars2environ(t_var *vars);

t_var	*get_env(const char *env_key);
const char	**split_first_c(const char *str, char c);
const char	**get_colon_units(const char *str, const char *default_str);
const char	*get_val_from_kvstr(const char *kvstr, char delimiter);
const char	*expand_env_var(const char *str);
const char	*generate_kvstr(const char *key, const char *value);
int		ft_setenv(const char *name, const char *value, int rewrite);
int		ft_unsetenv(const char *name);

// コマンドの終了ステータス
int		get_status(void);
void	set_status(int status_value);
int		set_status_and_ret(int status_value, int ret_value);

#endif
