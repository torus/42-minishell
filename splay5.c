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

void	splay_assign(t_splay_tree **var, t_splay_tree *val)
{
	t_splay_tree	*old_value;

	old_value = *var;
	*var = val;
	if (val)
		val->refcount++;
	splay_release(old_value);
}

void	splay_init(t_splay_tree **var, t_splay_tree *val)
{
	*var = val;
	if (val)
		val->refcount++;
}
