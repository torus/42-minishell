#include <stdlib.h>
#include "rope.h"

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

void	splay_path_assign(t_splay_path **var, t_splay_path *val)
{
	t_splay_path	*old_value;

	old_value = *var;
	*var = val;
	val->refcount++;
	splay_path_release(old_value);
}

void	splay_path_init(t_splay_path **var, t_splay_path *val)
{
	*var = val;
	if (val)
		val->refcount++;
}

t_splay_path	*splay_path_left(t_splay_path *path_orig)
{
	t_splay_tree	*current;
	t_splay_path	*path;

	path = path_orig;
	path->refcount++;
	current = path->node;
	if (current->left)
	{
		current = current->left;
		splay_path_assign(&path, splay_path_create(SPLAY_LEFT, current, path));
		while (current->right)
		{
			current = current->right;
			splay_path_assign(
				&path, splay_path_create(SPLAY_RIGHT, current, path));
		}
	}
	else if (path->dir == SPLAY_RIGHT)
		splay_path_assign(&path, path->next);
	else
		splay_path_assign(&path, path->next->next);
	path->refcount--;
	return (path);
}

t_splay_path	*splay_path_right(t_splay_path *path_orig)
{
	t_splay_tree	*current;
	t_splay_path	*path;

	path = path_orig;
	path->refcount++;
	current = path->node;
	if (current->right)
	{
		current = current->right;
		splay_path_assign(&path, splay_path_create(SPLAY_RIGHT, current, path));
		while (current->left)
		{
			current = current->left;
			splay_path_assign(
				&path, splay_path_create(SPLAY_LEFT, current, path));
		}
	}
	else if (path->dir == SPLAY_LEFT)
		splay_path_assign(&path, path->next);
	else
		splay_path_assign(&path, path->next->next);
	path->refcount--;
	return (path);
}
