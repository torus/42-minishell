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
	}

	TEST_SECTION("splay_insert_left");
	{
		t_splay_tree	*tree;
		char	*r = "root";
		char	*a = "a";
		char	*b = "b";

		tree = splay_create(NULL, r, NULL);
		tree = splay_insert_left(tree, a);
		CHECK_EQ(tree->value, a);

		tree = splay_insert_left(tree, b);
		CHECK_EQ(tree->value, b);
		CHECK_EQ(tree->right->value, a);
		CHECK_EQ(tree->right->right->value, r);
		CHECK_EQ(tree->right->right->right, NULL);
	}

	TEST_SECTION("splay_insert_right");
	{
		t_splay_tree	*tree;
		char	*r = "root";
		char	*a = "a";
		char	*b = "b";

		tree = splay_create(NULL, r, NULL);
		tree = splay_insert_right(tree, a);
		CHECK_EQ(tree->value, a);

		tree = splay_insert_right(tree, b);
		CHECK_EQ(tree->value, b);
		CHECK_EQ(tree->left->value, a);
		CHECK_EQ(tree->left->left->value, r);
		CHECK_EQ(tree->left->left->left, NULL);
	}

	TEST_SECTION("splay_path_create");
	{
		t_splay_path	*path;
		t_splay_tree	*tree;

		tree = splay_create(NULL, V(a), NULL);
		path = splay_path_create(SPLAY_ROOT, tree, NULL);

		CHECK(path);
		CHECK_EQ(path->dir, SPLAY_ROOT);
		CHECK_EQ(path->node, tree);
		CHECK_EQ(path->node->value, V(a));
		CHECK(!path->next);

		tree->right = splay_create(NULL, V(b), NULL);
		path = splay_path_create(SPLAY_RIGHT, tree->right, path);

		CHECK(path);
		CHECK_EQ(path->dir, SPLAY_RIGHT);
		CHECK_EQ(path->node, tree->right);
		CHECK_EQ(path->node->value, V(b));
		CHECK(path->next);
		CHECK_EQ(path->next->dir, SPLAY_ROOT);
		CHECK_EQ(path->next->node->value, V(a));
	}

	TEST_SECTION("splay_zig_right");
	{
		t_splay_tree	*x = splay_create(
			splay_create(NULL, V(A), NULL),
			V(x),
			splay_create(NULL, V(B), NULL));
		t_splay_tree	*p = splay_create(
			x,
			V(p),
			splay_create(NULL, V(C), NULL));

		t_splay_tree	*result = splay_zig_right(x, p);

		CHECK_EQ(result->value, V(x));
		CHECK_EQ(result->left->value, V(A));
		CHECK_EQ(result->right->value, V(p));
		CHECK_EQ(result->right->left->value, V(B));
		CHECK_EQ(result->right->right->value, V(C));
	}

	TEST_SECTION("splay_zig_zig_right");
	{
		t_splay_tree	*x = splay_create(
			splay_create(NULL, V(A), NULL),
			V(x),
			splay_create(NULL, V(B), NULL));
		t_splay_tree	*p = splay_create(
			x,
			V(p),
			splay_create(NULL, V(C), NULL));
		t_splay_tree	*g = splay_create(
			p,
			V(g),
			splay_create(NULL, V(D), NULL));

		t_splay_tree	*result = splay_zig_zig_right(x, p, g);

		CHECK_EQ(result->value, V(x));
		CHECK_EQ(result->left->value, V(A));
		CHECK_EQ(result->right->value, V(p));
		CHECK_EQ(result->right->left->value, V(B));
		CHECK_EQ(result->right->right->value, V(g));
		CHECK_EQ(result->right->right->left->value, V(C));
		CHECK_EQ(result->right->right->right->value, V(D));
	}

	TEST_SECTION("splay_zig_zag_right");
	{
		t_splay_tree	*x = splay_create(
			splay_create(NULL, V(B), NULL),
			V(x),
			splay_create(NULL, V(C), NULL));
		t_splay_tree	*p = splay_create(
			splay_create(NULL, V(A), NULL),
			V(p),
			x);
		t_splay_tree	*g = splay_create(
			p,
			V(g),
			splay_create(NULL, V(D), NULL));

		t_splay_tree	*result = splay_zig_zag_right(x, p, g);

		CHECK_EQ(result->value, V(x));
		CHECK_EQ(result->left->value, V(p));
		CHECK_EQ(result->left->left->value, V(A));
		CHECK_EQ(result->left->right->value, V(B));
		CHECK_EQ(result->right->value, V(g));
		CHECK_EQ(result->right->left->value, V(C));
		CHECK_EQ(result->right->right->value, V(D));
	}

	TEST_SECTION("splay_zig_left");
	{
		t_splay_tree	*x = splay_create(
			splay_create(NULL, V(B), NULL),
			V(x),
			splay_create(NULL, V(A), NULL));
		t_splay_tree	*p = splay_create(
			splay_create(NULL, V(C), NULL),
			V(p),
			x);

		t_splay_tree	*result = splay_zig_left(x, p);

		CHECK_EQ(result->value, V(x));
		CHECK_EQ(result->right->value, V(A));
		CHECK_EQ(result->left->value, V(p));
		CHECK_EQ(result->left->right->value, V(B));
		CHECK_EQ(result->left->left->value, V(C));
	}

	TEST_SECTION("splay_zig_zig_left");
	{
		t_splay_tree	*x = splay_create(
			splay_create(NULL, V(B), NULL),
			V(x),
			splay_create(NULL, V(A), NULL));
		t_splay_tree	*p = splay_create(
			splay_create(NULL, V(C), NULL),
			V(p),
			x);
		t_splay_tree	*g = splay_create(
			splay_create(NULL, V(D), NULL),
			V(g),
			p);

		t_splay_tree	*result = splay_zig_zig_left(x, p, g);

		CHECK_EQ(result->value, V(x));
		CHECK_EQ(result->right->value, V(A));
		CHECK_EQ(result->left->value, V(p));
		CHECK_EQ(result->left->right->value, V(B));
		CHECK_EQ(result->left->left->value, V(g));
		CHECK_EQ(result->left->left->right->value, V(C));
		CHECK_EQ(result->left->left->left->value, V(D));
	}

	TEST_SECTION("splay_zig_zag_left");
	{
		t_splay_tree	*x = splay_create(
			splay_create(NULL, V(C), NULL),
			V(x),
			splay_create(NULL, V(B), NULL));
		t_splay_tree	*p = splay_create(
			x,
			V(p),
			splay_create(NULL, V(A), NULL));
		t_splay_tree	*g = splay_create(
			splay_create(NULL, V(D), NULL),
			V(g),
			p);

		t_splay_tree	*result = splay_zig_zag_left(x, p, g);

		CHECK_EQ(result->value, V(x));
		CHECK_EQ(result->right->value, V(p));
		CHECK_EQ(result->right->right->value, V(A));
		CHECK_EQ(result->right->left->value, V(B));
		CHECK_EQ(result->left->value, V(g));
		CHECK_EQ(result->left->right->value, V(C));
		CHECK_EQ(result->left->left->value, V(D));
	}

	TEST_SECTION("splay zig 右");
	{
		t_splay_tree	*x = splay_create(
			splay_create(NULL, V(A), NULL),
			V(x),
			splay_create(NULL, V(B), NULL));
		t_splay_tree	*p = splay_create(
			x,
			V(p),
			splay_create(NULL, V(C), NULL));

		t_splay_path	*path = splay_path_create(
			SPLAY_LEFT, x, splay_path_create(SPLAY_ROOT, p, NULL));

		t_splay_tree	*result = splay(path);

		CHECK_EQ(result->value, V(x));
		CHECK_EQ(result->left->value, V(A));
		CHECK_EQ(result->right->value, V(p));
		CHECK_EQ(result->right->left->value, V(B));
		CHECK_EQ(result->right->right->value, V(C));
	}

	TEST_SECTION("splay zig 左");
	{
		t_splay_tree	*x = splay_create(
			splay_create(NULL, V(B), NULL),
			V(x),
			splay_create(NULL, V(A), NULL));
		t_splay_tree	*p = splay_create(
			splay_create(NULL, V(C), NULL),
			V(p),
			x);

		t_splay_path	*path = splay_path_create(
			SPLAY_RIGHT, x, splay_path_create(SPLAY_ROOT, p, NULL));

		t_splay_tree	*result = splay(path);

		CHECK_EQ(result->value, V(x));
		CHECK_EQ(result->right->value, V(A));
		CHECK_EQ(result->left->value, V(p));
		CHECK_EQ(result->left->right->value, V(B));
		CHECK_EQ(result->left->left->value, V(C));
	}

	TEST_SECTION("splay_path_left");
	{
		t_splay_tree	*a = splay_create(
			NULL, V(a),
			splay_create(
				NULL, V(b),
				splay_create(
					NULL, V(c), NULL)));

		t_splay_path	*path_b = splay_path_create(
			SPLAY_RIGHT, a->right,
			splay_path_create(SPLAY_ROOT, a, NULL));

		t_splay_path	*result = splay_path_left(path_b);

		CHECK_EQ(result->dir, SPLAY_ROOT);
		CHECK_EQ(result->node, a);
	}

	TEST_SECTION("splay_path_right");
	{
		t_splay_tree	*a = splay_create(
			NULL, V(a),
			splay_create(
				NULL, V(b),
				splay_create(
					NULL, V(c), NULL)));

		t_splay_path	*path_b = splay_path_create(
			SPLAY_RIGHT, a->right,
			splay_path_create(SPLAY_ROOT, a, NULL));

		t_splay_path	*result = splay_path_right(path_b);

		CHECK_EQ(result->dir, SPLAY_RIGHT);
		CHECK_EQ(result->node, a->right->right);
		CHECK_EQ(result->next, path_b);
	}

	TEST_SECTION("splay_insert_left");
	{
        t_splay_tree	*b = splay_create(
            splay_create(NULL, V(a), NULL),
            V(b),
            splay_create(NULL, V(c), NULL));

        t_splay_tree	*result = splay_insert_left(b, V(d));

        CHECK_EQ(result->value, V(d));
        CHECK_EQ(result->left->value, V(a));
        CHECK_EQ(result->right->right->value, V(c));
        CHECK_EQ(result->right->value, V(b));
	}

	TEST_SECTION("splay_insert_right");
	{
        t_splay_tree	*b = splay_create(
            splay_create(NULL, V(a), NULL),
            V(b),
            splay_create(NULL, V(c), NULL));

        t_splay_tree	*result = splay_insert_right(b, V(d));

        CHECK_EQ(result->value, V(d));
        CHECK_EQ(result->left->value, V(b));
        CHECK_EQ(result->left->left->value, V(a));
        CHECK_EQ(result->right->value, V(c));
	}
}

int main()
{
	test_tree();

	int fail_count = print_result();
	return (fail_count);
}
