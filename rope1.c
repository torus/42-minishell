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
		rope->value = (void *)val;
	}
	return ((int)val);
}
