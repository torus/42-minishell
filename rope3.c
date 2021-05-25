#include <stddef.h>
#include "rope.h"

#include <stdio.h>
static void	rope_set_right(t_rope *rope, t_rope *right)
{
	/* t_rope	*old_right; */

	/* old_right = rope->right; */
	splay_assign(&rope->right, right);
	/* rope->right = right; */
	/* if (right) */
	/* 	right->refcount++; */
	/* splay_release(old_right); */
}

static void	rope_split_update(
	t_rope *rope, t_rope *parent, t_rope **new_left, t_rope **new_right)
{
	/* t_rope	*old_left; */
	/* t_rope	*old_right; */

	rope->refcount++;

	/* old_left = NULL; */
	/* old_right = NULL; */
	/* splay_assign(&old_left, *new_left); */
	/* splay_assign(&old_right, *new_right); */
	/* old_right = *new_right; */
	rope->value = ROPE_NOWEIGHT;
	if (parent)
		rope_set_right(parent, rope->left);
	else
	{
		splay_assign(new_left, rope->left);
		/* *new_left = rope->left; */
		/* rope->left->refcount++; */
		/* splay_release(old_left); */
	}
	if (rope->right)
	{
		if (*new_right)
		{
			splay_assign(new_right, rope_concat(rope->right, *new_right));
			/* *new_right = rope_concat(rope->right, *new_right); */
			/* (*new_right)->refcount++; */
			/* splay_release(old_right); */
		}
		else
		{
			splay_assign(new_right, rope->right);
			/* *new_right = rope->right; */
			/* (*new_right)->refcount++; */
			/* splay_release(old_right); */
		}
		rope_set_right(rope, NULL);
	}

	splay_release(rope);
}

void	rope_split_iter(
	t_rope *rope, int index, t_rope **left, t_rope **right)
{
	t_rope	*new_left;
	t_rope	*new_right;
	t_rope	*parent;

	new_left = NULL;
	new_right = NULL;
	parent = NULL;
	splay_assign(&new_left, rope);
	while (1)
	{
		if (rope_weight(rope) < index && rope->right)
		{
			index = index - rope_weight(rope);
			/* splay_release(parent); */
			splay_assign(&parent, rope);
			parent->refcount++;
			splay_assign(&rope, rope->right);
		}
		else if (rope->left)
		{
			rope_split_update(rope, parent, &new_left, &new_right);
			new_left->refcount++;
			new_right->refcount++;
			splay_assign(&rope, rope->left);
		}
		else
			break ;
	}
	splay_release(parent);
	splay_assign(left, new_left);
	splay_assign(right, new_right);

	splay_release(new_left);
	splay_release(new_right);
	splay_release(parent);
	printf("split iter: new_left %d, new_right %d\n", new_left->refcount, new_right->refcount);
}

void	rope_split(t_rope *rope, int index, t_rope **left, t_rope **right)
{
	t_splay_path	*path;
	t_splay_path	*cur;
	t_rope			*splayed;

	splayed = NULL;
	path = NULL;
	rope_index_with_path(rope, index, &path);
	path->refcount++;
	cur = path->next;
	while (cur)
	{
		cur->node->value = ROPE_NOWEIGHT;
		cur = cur->next;
	}
	splay_assign(&splayed, splay(path->next));
	splayed->refcount++;
	rope_split_iter(splayed, index, left, right);
	splay_release(splayed);
	splay_path_release(path);
}
