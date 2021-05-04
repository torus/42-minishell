#include "rope.h"

/*
 *      p                      x
 *     / \         =>         / \
 *    A   x                  p   C
 *       / \                / \
 *      B   C              A   B
 */
t_splay_tree	*splay_zig_left(t_splay_tree *x, t_splay_tree *p)
{
	return (
		splay_create(
			splay_create(p->left, p->value, x->left), x->value, x->right));
}

/*
 *    g                          x
 *   / \                        / \
 *  A   p                      p   D
 *     / \         =>         / \
 *    B   x                  g   C
 *       / \                / \
 *      C   D              A   B
 */
t_splay_tree	*splay_zig_zig_left(
					t_splay_tree *x, t_splay_tree *p, t_splay_tree *g)
{
    t_splay_tree	*a;
    t_splay_tree	*b;
    t_splay_tree	*c;
    t_splay_tree	*d;

    a = g->left;
    b = p->left;
    c = x->left;
    d = x->right;
	return (
		splay_create(
			splay_create(
				splay_create(a, g->value, b),
                p->value, c),
			x->value, d));
}

/*
 *       g                      x
 *      / \                   /   \
 *     A   p                g       p
 *        / \        =>    / \     / \
 *       x   D            A   B   C   D
 *      / \
 *     B   C
 */
t_splay_tree	*splay_zig_zag_left(
					t_splay_tree *x, t_splay_tree *p, t_splay_tree *g)
{
	t_splay_tree	*a;
	t_splay_tree	*b;
	t_splay_tree	*c;
	t_splay_tree	*d;

	a = g->left;
	b = x->left;
	c = x->right;
	d = p->right;
	return (
		splay_create(
			splay_create(a, g->value, b),
			x->value,
            splay_create(c, p->value, d)));
}
