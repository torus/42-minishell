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
	t_rope	*parent;
	t_rope	*cur;

	parent = NULL;
	splay_init(&cur, rope);
	splay_init(left, cur);
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
			rope_split_update(cur, parent, left, right);
			splay_assign(&cur, cur->left);
		}
		else
			break ;
	}
	splay_release(parent);
	splay_release(cur);
}

static void	rope_split_aux(
	t_rope *rope, int index, t_rope **left, t_rope **right)
{
	if (index == 0)
	{
		splay_assign(left, NULL);
		splay_assign(right, rope);
		return ;
	}
	splay_assign(left, rope);
	splay_assign(right, NULL);
}

void	rope_split(t_rope *rope, int index, t_rope **left, t_rope **right)
{
	t_splay_path	*path;
	t_splay_path	*cur;
	t_rope			*splayed;

	if (index > 0 && index < rope_length(rope))
	{
		splayed = NULL;
		path = NULL;
		rope_index_with_path(rope, index, &path);
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
	else
		rope_split_aux(rope, index, left, right);
}
