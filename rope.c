#include <stdlib.h>
#include <stdint.h>
#include "libft/libft.h"
#include "rope.h"

t_rope	*rope_create(char *left, char *right)
{
	t_rope			*rope;
	t_splay_tree	*ltree;
	t_splay_tree	*rtree;

	ltree = NULL;
	if (left)
	{
		ltree = splay_create(NULL, NULL, NULL);
		ft_strlcpy((char *)&ltree->value, left, sizeof (void *));
	}
	rtree = NULL;
	if (right)
	{
		rtree = splay_create(NULL, NULL, NULL);
		ft_strlcpy((char *)&rtree->value, right, sizeof (void *));
	}
	rope = splay_create(ltree, ROPE_NOWEIGHT, rtree);
	return (rope);
}

t_rope	*rope_concat(t_rope *left, t_rope *right)
{
	return (splay_create(left, ROPE_NOWEIGHT, right));
}

int	rope_weight(t_rope *rope)
{
	int64_t	val;
	t_rope	*cur;

	if (!rope->left && !rope->right)
	{
		return (ft_strlen((char *)&rope->value));
	}
	val = (int64_t)rope->value;
	if (val == -1)
	{
		if (!rope->left)
			val = 0;
		else
		{
			val = rope_weight(rope->left);
			cur = rope->left;
			while (cur->right)
			{
				val += rope_weight(cur->right);
				cur = cur->right;
			}
		}
		rope->value = (void*)val;
	}
	return ((int)val);
}

char	rope_index_with_path_iter(t_rope *rope, int index, t_splay_path **path)
{
	const int	weight = rope_weight(rope);
	if (weight <= index && rope->right)
	{
		if (path)
			*path = splay_path_create(SPLAY_RIGHT, rope->right, *path);
		return (rope_index_with_path_iter(rope->right, index - weight, path));
	}
	if (rope->left)
	{
		if (path)
			*path = splay_path_create(SPLAY_LEFT, rope->left, *path);
		return (rope_index_with_path_iter(rope->left, index, path));
	}
	return (((char *)&rope->value)[index]);
}

char	rope_index_with_path(t_rope *rope, int index, t_splay_path **path)
{
	if (path)
		*path = splay_path_create(SPLAY_ROOT, rope, NULL);
	return (rope_index_with_path_iter(rope, index, path));
}

char	rope_index(t_rope *rope, int index)
{
	return (rope_index_with_path(rope, index, NULL));
}

int	rope_length(t_rope *rope)
{
	if (rope->right)
		return (rope_weight(rope) + rope_length(rope->right));
	return (rope_weight(rope));
}

void	rope_set_right(t_rope *rope, t_rope *right)
{
	rope->right = right;
}

void	rope_split_iter(
	t_rope *rope, int index, t_rope **left, t_rope **right)
{
	t_rope	*new_right;
	t_rope	*parent;
	t_rope	*new_root;

	new_right = NULL;
	parent = NULL;
	new_root = rope;

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
			rope->value = ROPE_NOWEIGHT;
			if (parent)
				rope_set_right(parent, rope->left);
			else
				new_root = rope->left;

			if (rope->right)
			{
				if (new_right)
					new_right = rope_concat(rope->right, new_right);
				else
					new_right = rope->right;
				rope_set_right(rope, NULL);
			}
			rope = rope->left;
		}
		else
		{
			*right = new_right;
			*left = new_root;
			return ;
		}
	}
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

t_rope	*rope_insert(t_rope *rope, int index, t_rope *new_rope)
{
	t_rope	*left;
	t_rope	*right;

	rope_split(rope, index, &left, &right);
	return (rope_concat(left, rope_concat(new_rope, right)));
}

t_rope	*rope_delete(t_rope *rope, int begin, int end)
{
	t_rope	*left;
	t_rope	*left_middle;
	t_rope	*middle;
	t_rope	*right;

	rope_split(rope, end, &left_middle, &right);
	rope_split(left_middle, begin, &left, &middle);
	return (rope_concat(left, right));
}
