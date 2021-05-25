#include <stddef.h>
#include "rope.h"

static char	rope_index_with_path_iter(
	t_rope *rope, int index, t_splay_path **path)
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
