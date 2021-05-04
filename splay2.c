#include "rope.h"

/*
 *     p                    x
 *    / \        =>        / \
 *   x   C                A   p
 *  / \                      / \
 * A   B                    B   C
 */
t_splay_tree	*splay_zig_right(t_splay_tree *x, t_splay_tree *p)
{
	return (
		splay_create(
			x->left, x->value, splay_create(x->right, p->value, p->right)));
}

/*
 *       g                x
 *      / \              / \
 *     p   D            A   p
 *    / \        =>        / \
 *   x   C                B   g
 *  / \                      / \
 * A   B                    C   D
 */
t_splay_tree	*splay_zig_zig_right(
					t_splay_tree *x, t_splay_tree *p, t_splay_tree *g)
{
	return (
		splay_create(
			x->left, x->value,
			splay_create(
				x->right, p->value,
				splay_create(p->right, g->value, g->right))));
}

/*
 *       g                  x
 *      / \               /   \
 *     p   D            p       g
 *    / \        =>    / \     / \
 *   A   x            A   B   C   D
 *      / \
 *     B   C
 */
t_splay_tree	*splay_zig_zag_right(
					t_splay_tree *x, t_splay_tree *p, t_splay_tree *g)
{
	t_splay_tree	*a;
	t_splay_tree	*b;
	t_splay_tree	*c;
	t_splay_tree	*d;

	a = p->left;
	b = x->left;
	c = x->right;
	d = g->right;
	return (
		splay_create(
			splay_create(a, p->value, b),
			x->value,
			splay_create(c, g->value, d)));
}
