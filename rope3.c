#include <stddef.h>
#include "rope.h"

#include <stdio.h>
static void	rope_set_right(t_rope *rope, t_rope *right)
{
	splay_assign(&rope->right, right);
}

static void	rope_split_update(
	t_rope *rope, t_rope *parent, t_rope **new_left, t_rope **new_right)
{
	t_rope	*backup;

	splay_init(&backup, rope);
	rope->value = ROPE_NOWEIGHT;
	if (parent)
		rope_set_right(parent, rope->left);
	else
		splay_assign(new_left, rope->left);
	if (rope->right)
	{
		if (*new_right)
			splay_assign(new_right, rope_concat(rope->right, *new_right));
		else
			splay_assign(new_right, rope->right);
		rope_set_right(rope, NULL);
	}
	splay_release(backup);
}

void	rope_split_iter(
	t_rope *rope, int index, t_rope **left, t_rope **right)
{
	t_rope	*new_left;
	t_rope	*new_right;
	t_rope	*parent;
	t_rope	*cur;

	new_right = NULL;
	parent = NULL;
	splay_init(&cur, rope);
	splay_init(&new_left, cur);
	while (1)
	{
		if (rope_weight(cur) < index && cur->right)
		{
			index = index - rope_weight(cur);
			splay_assign(&parent, cur);
			splay_assign(&cur, cur->right);
		}
		else if (cur->left)
		{
			rope_split_update(cur, parent, &new_left, &new_right);
			splay_assign(&cur, cur->left);
		}
		else
			break ;
	}
	splay_assign(left, new_left);
	splay_assign(right, new_right);

	splay_release(new_left);
	splay_release(new_right);
	splay_release(parent);
	splay_release(cur);
}

void	rope_split(t_rope *rope, int index, t_rope **left, t_rope **right)
{
	t_splay_path	*path;
	t_splay_path	*cur;
	t_rope			*splayed;

	splayed = NULL;
	path = NULL;
	rope_index_with_path(rope, index, &path);
	/* path->refcount++; */
	cur = path->next;
	while (cur)
	{
		cur->node->value = ROPE_NOWEIGHT;
		cur = cur->next;
	}
	splay_assign(&splayed, splay(path->next));
	rope_split_iter(splayed, index, left, right);
	splay_release(splayed);
	splay_path_release(path);
}
