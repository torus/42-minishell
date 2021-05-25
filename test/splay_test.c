#include "test.h"
#include "../rope.h"

#define V(x) ((void*)((unsigned long)(#x)[0]))

void test_tree()
{
	TEST_CHAPTER("スプレー木");

	TEST_SECTION("splay_create");
	{
		t_splay_tree	*tree;
		char	*val = "hello";

		tree = splay_create(NULL, val, NULL);
		CHECK(tree);
		CHECK_EQ(tree->value, val);
		CHECK(!tree->left);
		CHECK(!tree->right);
		splay_release(tree);
	}

	TEST_SECTION("splay_insert_left");
	{
		t_splay_tree	*tree;
		char	*r = "root";
		char	*a = "a";
		char	*b = "b";

		tree = NULL;
		splay_assign(&tree, splay_create(NULL, r, NULL));
		splay_assign(&tree, splay_insert_left(tree, a));
		CHECK_EQ(tree->value, a);

		splay_assign(&tree, splay_insert_left(tree, b));
		CHECK_EQ(tree->value, b);
		CHECK_EQ(tree->right->value, a);
		CHECK_EQ(tree->right->right->value, r);
		CHECK_EQ(tree->right->right->right, NULL);
		splay_release(tree);
	}

	TEST_SECTION("splay_insert_right");
	{
		t_splay_tree	*tree;
		char	*r = "root";
		char	*a = "a";
		char	*b = "b";

		tree = splay_create(NULL, r, NULL);
		splay_assign(&tree, splay_insert_right(tree, a));
		CHECK_EQ(tree->value, a);

		splay_assign(&tree, splay_insert_right(tree, b));
		CHECK_EQ(tree->value, b);
		CHECK_EQ(tree->left->value, a);
		CHECK_EQ(tree->left->left->value, r);
		CHECK_EQ(tree->left->left->left, NULL);
		splay_release(tree);
	}

	TEST_SECTION("splay_path_create");
	{
		t_splay_path	*path;
		t_splay_tree	*tree;

		tree = splay_create(NULL, V(a), NULL);
		tree->refcount++;
		path = splay_path_create(SPLAY_ROOT, tree, NULL);
		path->refcount++;

		CHECK(path);
		CHECK_EQ(path->dir, SPLAY_ROOT);
		CHECK_EQ(path->node, tree);
		CHECK_EQ(path->node->value, V(a));
		CHECK(!path->next);

		splay_assign(&tree->right, splay_create(NULL, V(b), NULL));
		splay_path_assign(
			&path, splay_path_create(SPLAY_RIGHT, tree->right, path));

		CHECK(path);
		CHECK_EQ(path->refcount, 1);
		CHECK_EQ(path->next->refcount, 1);
		CHECK_EQ(path->dir, SPLAY_RIGHT);
		CHECK_EQ(path->node, tree->right);
		CHECK_EQ(path->node->value, V(b));
		CHECK(path->next);
		CHECK_EQ(path->next->dir, SPLAY_ROOT);
		CHECK_EQ(path->next->node->value, V(a));

		splay_release(tree);
		splay_path_release(path);
	}

	TEST_SECTION("splay_zig_right");
	{
		t_splay_tree	*x = splay_create(
			splay_create(NULL, V(A), NULL),
			V(x),
			splay_create(NULL, V(B), NULL));
		x->refcount++;
		t_splay_tree	*p = splay_create(
			x,
			V(p),
			splay_create(NULL, V(C), NULL));
		p->refcount++;
		t_splay_tree	*result = splay_zig_right(x, p);
		CHECK_EQ(result->refcount, 0);
		result->refcount++;

		CHECK_EQ(result->value, V(x));
		CHECK_EQ(result->left->value, V(A));
		CHECK_EQ(result->right->value, V(p));
		CHECK_EQ(result->right->left->value, V(B));
		CHECK_EQ(result->right->right->value, V(C));
		splay_release(result);
		splay_release(x);
		splay_release(p);
	}

	TEST_SECTION("splay_zig_zig_right");
	{
		t_splay_tree	*x = splay_create(
			splay_create(NULL, V(A), NULL),
			V(x),
			splay_create(NULL, V(B), NULL));
		x->refcount++;
		t_splay_tree	*p = splay_create(
			x,
			V(p),
			splay_create(NULL, V(C), NULL));
		p->refcount++;
		t_splay_tree	*g = splay_create(
			p,
			V(g),
			splay_create(NULL, V(D), NULL));
		g->refcount++;

		t_splay_tree	*result = splay_zig_zig_right(x, p, g);
		CHECK_EQ(result->refcount, 0);

		CHECK_EQ(result->value, V(x));
		CHECK_EQ(result->left->value, V(A));
		CHECK_EQ(result->right->value, V(p));
		CHECK_EQ(result->right->left->value, V(B));
		CHECK_EQ(result->right->right->value, V(g));
		CHECK_EQ(result->right->right->left->value, V(C));
		CHECK_EQ(result->right->right->right->value, V(D));

		splay_release(result);
		splay_release(x);
		splay_release(p);
		splay_release(g);
	}

	TEST_SECTION("splay_zig_zag_right");
	{
		t_splay_tree	*x = splay_create(
			splay_create(NULL, V(B), NULL),
			V(x),
			splay_create(NULL, V(C), NULL));
		x->refcount++;
		t_splay_tree	*p = splay_create(
			splay_create(NULL, V(A), NULL),
			V(p),
			x);
		p->refcount++;
		t_splay_tree	*g = splay_create(
			p,
			V(g),
			splay_create(NULL, V(D), NULL));
		g->refcount++;

		t_splay_tree	*result = splay_zig_zag_right(x, p, g);
		CHECK_EQ(result->refcount, 0);

		CHECK_EQ(result->value, V(x));
		CHECK_EQ(result->left->value, V(p));
		CHECK_EQ(result->left->left->value, V(A));
		CHECK_EQ(result->left->right->value, V(B));
		CHECK_EQ(result->right->value, V(g));
		CHECK_EQ(result->right->left->value, V(C));
		CHECK_EQ(result->right->right->value, V(D));
		splay_release(result);
		splay_release(x);
		splay_release(p);
		splay_release(g);
	}

	TEST_SECTION("splay_zig_left");
	{
		t_splay_tree	*x = splay_create(
			splay_create(NULL, V(B), NULL),
			V(x),
			splay_create(NULL, V(A), NULL));
		x->refcount++;
		t_splay_tree	*p = splay_create(
			splay_create(NULL, V(C), NULL),
			V(p),
			x);
		p->refcount++;

		t_splay_tree	*result = splay_zig_left(x, p);
		CHECK_EQ(result->refcount, 0);

		CHECK_EQ(result->value, V(x));
		CHECK_EQ(result->right->value, V(A));
		CHECK_EQ(result->left->value, V(p));
		CHECK_EQ(result->left->right->value, V(B));
		CHECK_EQ(result->left->left->value, V(C));
		splay_release(result);
		splay_release(x);
		splay_release(p);
	}

	TEST_SECTION("splay_zig_zig_left");
	{
		t_splay_tree	*x = splay_create(
			splay_create(NULL, V(B), NULL),
			V(x),
			splay_create(NULL, V(A), NULL));
		x->refcount++;
		t_splay_tree	*p = splay_create(
			splay_create(NULL, V(C), NULL),
			V(p),
			x);
		p->refcount++;
		t_splay_tree	*g = splay_create(
			splay_create(NULL, V(D), NULL),
			V(g),
			p);
		g->refcount++;

		t_splay_tree	*result = splay_zig_zig_left(x, p, g);
		CHECK_EQ(result->refcount, 0);

		CHECK_EQ(result->value, V(x));
		CHECK_EQ(result->right->value, V(A));
		CHECK_EQ(result->left->value, V(p));
		CHECK_EQ(result->left->right->value, V(B));
		CHECK_EQ(result->left->left->value, V(g));
		CHECK_EQ(result->left->left->right->value, V(C));
		CHECK_EQ(result->left->left->left->value, V(D));
		splay_release(result);
		splay_release(x);
		splay_release(p);
		splay_release(g);
	}

	TEST_SECTION("splay_zig_zag_left");
	{
		t_splay_tree	*x = splay_create(
			splay_create(NULL, V(C), NULL),
			V(x),
			splay_create(NULL, V(B), NULL));
		x->refcount++;
		t_splay_tree	*p = splay_create(
			x,
			V(p),
			splay_create(NULL, V(A), NULL));
		p->refcount++;
		t_splay_tree	*g = splay_create(
			splay_create(NULL, V(D), NULL),
			V(g),
			p);
		g->refcount++;

		t_splay_tree	*result = splay_zig_zag_left(x, p, g);
		CHECK_EQ(result->refcount, 0);

		CHECK_EQ(result->value, V(x));
		CHECK_EQ(result->right->value, V(p));
		CHECK_EQ(result->right->right->value, V(A));
		CHECK_EQ(result->right->left->value, V(B));
		CHECK_EQ(result->left->value, V(g));
		CHECK_EQ(result->left->right->value, V(C));
		CHECK_EQ(result->left->left->value, V(D));
		splay_release(result);
		splay_release(x);
		splay_release(p);
		splay_release(g);
	}

	TEST_SECTION("splay zig 右");
	{
		t_splay_tree	*x = splay_create(
			splay_create(NULL, V(A), NULL),
			V(x),
			splay_create(NULL, V(B), NULL));
		x->refcount++;
		t_splay_tree	*p = splay_create(
			x,
			V(p),
			splay_create(NULL, V(C), NULL));
		p->refcount++;

		t_splay_path	*path = splay_path_create(
			SPLAY_LEFT, x, splay_path_create(SPLAY_ROOT, p, NULL));
		path->refcount++;

		t_splay_tree	*result = splay(path);
		CHECK(result);
		CHECK_EQ(result->refcount, 0);

		CHECK_EQ(result->value, V(x));
		CHECK_EQ(result->left->value, V(A));
		CHECK_EQ(result->right->value, V(p));
		CHECK_EQ(result->right->left->value, V(B));
		CHECK_EQ(result->right->right->value, V(C));
		splay_release(result);
		splay_path_release(path);
		splay_release(x);
		splay_release(p);
	}

	TEST_SECTION("splay zig 左");
	{
		t_splay_tree	*x = splay_create(
			splay_create(NULL, V(B), NULL),
			V(x),
			splay_create(NULL, V(A), NULL));
		x->refcount++;
		t_splay_tree	*p = splay_create(
			splay_create(NULL, V(C), NULL),
			V(p),
			x);
		p->refcount++;

		printf("*** 0 x->refcount: %d\n", x->refcount);
		printf("*** 0 p->refcount: %d\n", p->refcount);

		t_splay_path	*path = splay_path_create(
			SPLAY_RIGHT, x, splay_path_create(SPLAY_ROOT, p, NULL));
		CHECK_EQ(path->refcount, 0);
		path->refcount++;
		printf("*** 1 x->refcount: %d\n", x->refcount);
		printf("*** 1 p->refcount: %d\n", p->refcount);

		t_splay_tree	*result = splay(path);
		CHECK_EQ(result->refcount, 0);
		result->refcount++;
		printf("*** 2 x->refcount: %d\n", x->refcount);
		printf("*** 2 p->refcount: %d\n", p->refcount);

		printf("path->refcount: %d\n", path->refcount);
		CHECK_EQ(path->refcount, 1);

		CHECK_EQ(result->value, V(x));
		CHECK_EQ(result->right->value, V(A));
		CHECK_EQ(result->left->value, V(p));
		CHECK_EQ(result->left->right->value, V(B));
		CHECK_EQ(result->left->left->value, V(C));
		CHECK_EQ(path->refcount, 1);
		splay_path_release(path);
		printf("*** 3 x->refcount: %d\n", x->refcount);
		printf("*** 3 p->refcount: %d\n", p->refcount);
		printf("result->refcount: %d\n", result->refcount);
		CHECK_EQ(result->refcount, 1);
		splay_release(result);
		printf("*** 4 x->refcount: %d\n", x->refcount);
		printf("*** 4 p->refcount: %d\n", p->refcount);
		splay_release(x);
		splay_release(p);
	}

	TEST_SECTION("splay_path_left");
	{
		t_splay_tree	*a = splay_create(
			NULL, V(a),
			splay_create(
				NULL, V(b),
				splay_create(
					NULL, V(c), NULL)));
		a->refcount++;

		t_splay_path	*path_b = splay_path_create(
			SPLAY_RIGHT, a->right,
			splay_path_create(SPLAY_ROOT, a, NULL));
		path_b->refcount++;

		t_splay_path	*result = splay_path_left(path_b);
		result->refcount++;

		CHECK_EQ(result->dir, SPLAY_ROOT);
		CHECK_EQ(result->node, a);

		splay_path_release(result);
		splay_path_release(path_b);
		splay_release(a);
	}

	TEST_SECTION("splay_path_right");
	{
		t_splay_tree	*a = splay_create(
			NULL, V(a),
			splay_create(
				NULL, V(b),
				splay_create(
					NULL, V(c), NULL)));
		a->refcount++;

		t_splay_path	*path_b = splay_path_create(
			SPLAY_RIGHT, a->right,
			splay_path_create(SPLAY_ROOT, a, NULL));
		path_b->refcount++;

		printf("refcount path_b: %d, a: %d\n", path_b->refcount, a->refcount);

		t_splay_path	*result = splay_path_right(path_b);
		result->refcount++;

		printf("refcount path_b: %d, a: %d\n", path_b->refcount, a->refcount);

		CHECK_EQ(result->dir, SPLAY_RIGHT);
		CHECK_EQ(result->node, a->right->right);
		CHECK_EQ(result->next, path_b);

		printf("refcount result: %d, path_b: %d, a: %d\n", result->refcount, path_b->refcount, a->refcount);

		splay_path_release(result);
		printf("* path_b: %d, a: %d\n", path_b->refcount, a->refcount);
		splay_path_release(path_b);

		printf("refcount a: %d, a->right: %d, a->right->right: %d\n",
			   a->refcount, a->right->refcount, a->right->right->refcount);
		splay_release(a);
	}

	TEST_SECTION("splay_insert_left");
	{
		t_splay_tree	*b = splay_create(
			splay_create(NULL, V(a), NULL),
			V(b),
			splay_create(NULL, V(c), NULL));
		b->refcount++;

		t_splay_tree	*result = splay_insert_left(b, V(d));
		result->refcount++;

		CHECK_EQ(result->value, V(d));
		CHECK_EQ(result->left->value, V(a));
		CHECK_EQ(result->right->right->value, V(c));
		CHECK_EQ(result->right->value, V(b));

		splay_release(b);
		splay_release(result);
	}

	TEST_SECTION("splay_insert_right");
	{
		t_splay_tree	*b = splay_create(
			splay_create(NULL, V(a), NULL),
			V(b),
			splay_create(NULL, V(c), NULL));
		b->refcount++;

		t_splay_tree	*result = splay_insert_right(b, V(d));
		result->refcount++;

		CHECK_EQ(result->value, V(d));
		CHECK_EQ(result->left->value, V(b));
		CHECK_EQ(result->left->left->value, V(a));
		CHECK_EQ(result->right->value, V(c));

		splay_release(b);
		splay_release(result);
	}
}

int main()
{
	test_tree();

	int fail_count = print_result();
	return (fail_count);
}
