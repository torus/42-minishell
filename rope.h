typedef struct s_splay_tree	t_splay_tree;
struct	s_splay_tree
{
	void	*value;
	t_splay_tree	*left;
	t_splay_tree	*right;
};

typedef enum	e_splay_direction
{
	SPLAY_ROOT = 0x500,
	SPLAY_LEFT,
	SPLAY_RIGHT,
}	t_splay_direction;

typedef struct s_splay_path	t_splay_path;
struct	s_splay_path
{
	t_splay_direction	dir;
	t_splay_tree		*node;
	t_splay_path		*next;
};

t_splay_tree	*splay_create(
					t_splay_tree *left, void *value, t_splay_tree *right);
t_splay_tree	*splay_insert_left(
					t_splay_tree *tree, void *value);
t_splay_tree	*splay_insert_right(
					t_splay_tree *tree, void *value);

t_splay_path	*splay_path_create(t_splay_direction dir, t_splay_tree *node,
					t_splay_path *next);

t_splay_tree	*splay_zig_right(t_splay_tree *x, t_splay_tree *p);
t_splay_tree	*splay_zig_zig_right(
					t_splay_tree *x, t_splay_tree *p, t_splay_tree *g);
t_splay_tree	*splay_zig_zag_right(
					t_splay_tree *x, t_splay_tree *p, t_splay_tree *g);

t_splay_tree	*splay_zig_left(t_splay_tree *x, t_splay_tree *p);
t_splay_tree	*splay_zig_zig_left(
					t_splay_tree *x, t_splay_tree *p, t_splay_tree *g);
t_splay_tree	*splay_zig_zag_left(
					t_splay_tree *x, t_splay_tree *p, t_splay_tree *g);

t_splay_tree	*splay(t_splay_path *path);
t_splay_path	*splay_path_left(t_splay_path *path);
t_splay_path	*splay_path_right(t_splay_path *path);
