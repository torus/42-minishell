#include <stdlib.h>
#include "rope.h"

t_splay_tree	*splay_create(
					t_splay_tree *left, void *value, t_splay_tree *right)
{
	t_splay_tree	*tree;

	tree = malloc(sizeof(t_splay_tree));
	if (tree)
	{
		tree->left = left;
		if (left)
			left->refcount++;
		tree->right = right;
		if (right)
			right->refcount++;
		tree->value = value;
		tree->refcount = 0;
	}
	return (tree);
}

t_splay_tree	*splay_insert_left(
					t_splay_tree *tree, void *value)
{
	return (
		splay_create(
			tree->left, value, splay_create(NULL, tree->value, tree->right)));
}

t_splay_tree	*splay_insert_right(
					t_splay_tree *tree, void *value)
{
	return (
		splay_create(
			splay_create(tree->left, tree->value, NULL), value, tree->right));
}

t_splay_path	*splay_path_create(t_splay_direction dir, t_splay_tree *node,
					t_splay_path *next)
{
	t_splay_path	*path;

	path = malloc(sizeof(t_splay_path));
	if (path)
	{
		path->dir = dir;
		path->node = node;
		node->refcount++;
		path->next = next;
		if (next)
			next->refcount++;
		path->refcount = 0;
	}
	return (path);
}
