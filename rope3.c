#include <stddef.h>
#include "rope.h"

static void	rope_set_right(t_rope *rope, t_rope *right)
{
	rope->right = right;
}

static void	rope_split_update(
	t_rope *rope, t_rope *parent, t_rope **new_left, t_rope **new_right)
{
	rope->value = ROPE_NOWEIGHT;
	if (parent)
		rope_set_right(parent, rope->left);
	else
		*new_left = rope->left;
	if (rope->right)
	{
		if (*new_right)
			*new_right = rope_concat(rope->right, *new_right);
		else
			*new_right = rope->right;
		rope_set_right(rope, NULL);
	}
}

void	rope_split_iter(
	t_rope *rope, int index, t_rope **left, t_rope **right)
{
	t_rope	*new_left;
	t_rope	*new_right;
	t_rope	*parent;

	new_right = NULL;
	parent = NULL;
	new_left = rope;
	while (1)
	{
		if (rope_weight(rope) < index && rope->right)
		{
			index = index - rope_weight(rope);
			parent = rope;
			rope = rope->right;
		}
		else if (rope->left)
		{
			rope_split_update(rope, parent, &new_left, &new_right);
			rope = rope->left;
		}
		else
			break ;
	}
	*left = new_left;
	*right = new_right;
}

void	rope_split(t_rope *rope, int index, t_rope **left, t_rope **right)
{
	t_splay_path	*path;
	t_splay_path	*cur;
	t_rope			*splayed;

	rope_index_with_path(rope, index, &path);
	cur = path->next;
	while (cur)
	{
		cur->node->value = ROPE_NOWEIGHT;
		cur = cur->next;
	}
	splayed = splay(path->next);
	rope_split_iter(splayed, index, left, right);
}
