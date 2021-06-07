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
	t_rope	*l;
	t_rope	*r;
	t_rope	*result;

	if (!left && !right)
		return (NULL);
	splay_init(&l, left);
	splay_init(&r, right);
	if (l && !l->right && l->left)
		splay_assign(&l, l->left);
	else if (l && l->right && !l->left)
		splay_assign(&l, l->right);
	if (r && !r->right && r->left)
		splay_assign(&r, r->left);
	else if (r && r->right && !r->left)
		splay_assign(&r, r->right);
	splay_init(&result, splay_create(l, ROPE_NOWEIGHT, r));
	splay_release(l);
	splay_release(r);
	result->refcount--;
	return (result);
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
		rope->value = (void *)val;
	}
	return ((int)val);
}
