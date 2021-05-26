#include <stddef.h>
#include "rope.h"

t_rope	*rope_insert(t_rope *rope, int index, t_rope *new_rope)
{
	t_rope	*left;
	t_rope	*right;
	t_rope	*result;

	left = NULL;
	right = NULL;
	rope_split(rope, index, &left, &right);
	splay_init(&result, rope_concat(left, rope_concat(new_rope, right)));
	splay_release(left);
	splay_release(right);
	result->refcount--;
	return (result);
}

t_rope	*rope_delete(t_rope *rope, int begin, int end)
{
	t_rope	*left;
	t_rope	*left_middle;
	t_rope	*middle;
	t_rope	*right;
	t_rope	*result;

	left = NULL;
	left_middle = NULL;
	middle = NULL;
	right = NULL;
	rope_split(rope, end, &left_middle, &right);
	rope_split(left_middle, begin, &left, &middle);
	splay_init(&result, rope_concat(left, right));
	splay_release(left);
	splay_release(right);
	splay_release(left_middle);
	splay_release(middle);
	result->refcount--;
	return (result);
}
