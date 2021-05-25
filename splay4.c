#include "rope.h"

static t_splay_tree	*splay_1(t_splay_path *path)
{
	t_splay_tree		*x;
	t_splay_tree		*p;
	t_splay_direction	d1;
	t_splay_direction	d2;
	t_splay_path		*new_path;

	x = path->node;
	p = path->next->node;
	d1 = path->dir;
	d2 = path->next->dir;
	if (d1 == SPLAY_LEFT)
		new_path = splay_path_create(
				d2, splay_zig_right(x, p), path->next->next);
	else
		new_path = splay_path_create(
				d2, splay_zig_left(x, p), path->next->next);
	new_path->refcount++;

	t_splay_tree	*result;

	result = splay(new_path);
	result->refcount++;
	splay_path_release(new_path);
	result->refcount--;
	return (result);
}

static t_splay_tree	*splay_2(t_splay_path *path)
{
	t_splay_tree		*x;
	t_splay_tree		*p;
	t_splay_tree		*g;
	t_splay_tree		*rotated;

	x = path->node;
	p = path->next->node;
	g = path->next->next->node;
	if (path->dir == SPLAY_LEFT)
	{
		if (path->next->dir == SPLAY_LEFT)
			rotated = splay_zig_zig_right(x, p, g);
		else
			rotated = splay_zig_zag_left(x, p, g);
	}
	else
	{
		if (path->next->dir == SPLAY_RIGHT)
			rotated = splay_zig_zig_left(x, p, g);
		else
			rotated = splay_zig_zag_right(x, p, g);
	}

	t_splay_tree	*result;
	t_splay_path	*new_path;

	new_path = splay_path_create(
		path->next->next->dir, rotated, path->next->next->next);
	new_path->refcount++;
	result = splay(new_path);
	result->refcount++;
	splay_path_release(new_path);
	result->refcount--;
	return (result);
}

t_splay_tree	*splay(t_splay_path *path)
{
	if (path->next)
	{
		if (path->next->next)
			return (splay_2(path));
		return (splay_1(path));
	}
	return (path->node);
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
