#include "rope.h"

static t_splay_tree	*splay_1_result(t_splay_path *new_path)
{
	t_splay_tree	*result;

	new_path->refcount++;
	result = splay(new_path);
	result->refcount++;
	splay_path_release(new_path);
	result->refcount--;
	return (result);
}

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
	return (splay_1_result(new_path));
}

static t_splay_tree	*splay_2_result(t_splay_path *path, t_splay_tree *rotated)
{
	t_splay_tree	*result;
	t_splay_path	*new_path;

	splay_path_init(
		&new_path, splay_path_create(
			path->next->next->dir, rotated, path->next->next->next));
	splay_init(&result, splay(new_path));
	splay_path_release(new_path);
	splay_release(rotated);
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
			splay_init(&rotated, splay_zig_zig_right(x, p, g));
		else
			splay_init(&rotated, splay_zig_zag_left(x, p, g));
	}
	else
	{
		if (path->next->dir == SPLAY_RIGHT)
			splay_init(&rotated, splay_zig_zig_left(x, p, g));
		else
			splay_init(&rotated, splay_zig_zag_right(x, p, g));
	}
	return (splay_2_result(path, rotated));
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
