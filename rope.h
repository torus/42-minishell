#ifndef ROPE_H
# define ROPE_H

typedef struct s_splay_tree	t_splay_tree;
struct	s_splay_tree
{
	void			*value;
	t_splay_tree	*left;
	t_splay_tree	*right;
	int				refcount;
};

typedef enum e_splay_direction
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
	int					refcount;
};

void			splay_release(t_splay_tree *tree);
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
void			splay_path_release(t_splay_path *path);

void			splay_assign(t_splay_tree **var, t_splay_tree *val);
void			splay_init(t_splay_tree **var, t_splay_tree *val);
void			splay_path_assign(t_splay_path **var, t_splay_path *val);

# define ROPE_NOWEIGHT ((void*)-1L)

typedef struct s_splay_tree	t_rope;

t_rope			*rope_create(char *left, char *right);
t_rope			*rope_concat(t_rope *left, t_rope *right);
int				rope_weight(t_rope *rope);
char			rope_index(t_rope *rope, int index);
char			rope_index_with_path(
					t_rope *rope, int index, t_splay_path **path);
int				rope_length(t_rope *rope);
void			rope_split(
					t_rope *rope, int index, t_rope **left, t_rope **right);
t_rope			*rope_insert(t_rope *rope, int index, t_rope *right);
t_rope			*rope_delete(t_rope *rope, int begin, int end);

#endif
