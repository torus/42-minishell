#include <stdlib.h>
#include "rope.h"

void	splay_release(t_splay_tree *tree)
{
	if (!tree)
		return ;
	tree->refcount--;
	if (tree->refcount <= 0)
	{
		splay_release(tree->left);
		splay_release(tree->right);
		free (tree);
	}
}

void	splay_path_release(t_splay_path *path)
{
	if (!path)
		return ;
	path->refcount--;
	if (path->refcount <= 0)
	{
		splay_path_release(path->next);
		splay_release(path->node);
		free(path);
	}
}

void	splay_assign(t_splay_tree **var, t_splay_tree *val)
{
	t_splay_tree	*old_value;

	old_value = *var;
	*var = val;
	if (val)
		val->refcount++;
	splay_release(old_value);
}

void	splay_path_assign(t_splay_path **var, t_splay_path *val)
{
	t_splay_path	*old_value;

	old_value = *var;
	*var = val;
	val->refcount++;
	splay_path_release(old_value);
}
