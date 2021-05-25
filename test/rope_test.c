#include "test.h"
#include "../rope.h"

t_rope	*brownfox()
{
	t_rope	*r01 = rope_create("q", "u");
	t_rope	*r02 = rope_create("i", "c");
	t_rope	*r03 = rope_create("k", " ");
	t_rope	*r04 = rope_create("b", "r");
	t_rope	*r05 = rope_create("o", "w");
	t_rope	*r06 = rope_create("n", " ");
	t_rope	*r07 = rope_create("f", "o");
	t_rope	*r08 = rope_create("x", " ");
	t_rope	*r09 = rope_create("j", "u");
	t_rope	*r10 = rope_create("m", "p");
	t_rope	*r11 = rope_create("s", NULL);

	t_rope	*rope =
		rope_concat(
			rope_concat(
				rope_concat(
					rope_concat(r01, r02), rope_concat(r03, r04)),
				rope_concat(
					rope_concat(r05, r06), rope_concat(r07, r08))),
			rope_concat(
				r09, rope_concat(r10, r11)));

	return (rope);
}

void	test_rope()
{
	TEST_CHAPTER("ロープ");

	TEST_SECTION("rope_create, rope_index, rope_length");
	{
		t_rope	*rope = rope_create("Hell", "o");

		CHECK(rope);
		CHECK_EQ(rope_weight(rope->left), 4);
		CHECK_EQ(rope_weight(rope->right), 1);
		CHECK_EQ(rope_weight(rope), 4);
		CHECK_EQ(rope_index(rope, 0), 'H');
		CHECK_EQ(rope_index(rope, 3), 'l');
		CHECK_EQ(rope_index(rope, 4), 'o');
		CHECK_EQ(rope_length(rope), 5);
		splay_release(rope);
	}

	TEST_SECTION("rope_concat");
	{
		t_rope	*left = rope_create("Hell", "o");
		t_rope	*right = rope_create(", ", "World!");
		t_rope	*rope = rope_concat(left, right);

		// Hello, World!

		CHECK(rope);
		CHECK_EQ(rope_length(rope), 13);
		CHECK_EQ(rope_index(rope, 4), 'o');
		CHECK_EQ(rope_index(rope, 5), ',');

		splay_release(rope);
	}

	TEST_SECTION("rope_weight");
	{
		t_rope	*left = rope_create("Hell", "o");
		t_rope	*right = rope_create(", ", "World!");
		t_rope	*rope = rope_concat(left, right);

		CHECK(rope);
		CHECK_EQ(rope_weight(left), 4);
		CHECK_EQ(rope_weight(right), 2);
		CHECK_EQ(rope_weight(rope), 5);

		splay_release(rope);
	}

	TEST_SECTION("splay");
	{
		t_rope	*rope = brownfox();
		rope->refcount++;

		t_splay_path	*path;
		t_splay_path	*cur;
		t_rope			*splayed;

		path = NULL;
		rope_index_with_path(rope, 9, &path);
		path->refcount++;
		cur = path->next;
		while (cur)
		{
			cur->node->value = ROPE_NOWEIGHT;
			cur = cur->next;
		}
		splayed = splay(path->next);
		splayed->refcount++;

		CHECK_EQ(rope_index(splayed, 0), 'q');
		CHECK_EQ(rope_index(splayed, 9), 'w');
		CHECK_EQ(rope_index(splayed, 10), 'n');
		CHECK_EQ(rope_index(splayed, 20), 's');
		CHECK_EQ(rope_length(splayed), 21);

		splay_release(rope);
		splay_release(splayed);
		splay_path_release(path);
	}

	TEST_SECTION("rope_split");
	{
		t_rope	*rope = brownfox();
		rope->refcount++;
		// quick brown fox jumps
		CHECK(rope);
		CHECK_EQ(rope_index(rope, 0), 'q');
		CHECK_EQ(rope_index(rope, 9), 'w');
		CHECK_EQ(rope_index(rope, 10), 'n');
		CHECK_EQ(rope_index(rope, 20), 's');
		CHECK_EQ(rope_length(rope), 21);

		t_rope	*left = NULL, *right = NULL;
		rope_split(rope, 9, &left, &right);
		left->refcount++;
		right->refcount++;

		CHECK(left);
		CHECK_EQ(rope_length(left), 9);
		CHECK(right);
		CHECK_EQ(rope_length(right), 12);

		CHECK_EQ(rope_index(left, 0), 'q');
		CHECK_EQ(rope_index(left, 8), 'o');
		CHECK_EQ(rope_index(right, 0), 'w');
		CHECK_EQ(rope_index(right, 11), 's');

		printf("refcount: rope %d, left %d right %d\n", rope->refcount, left->refcount, right->refcount);

		splay_release(left);
		splay_release(right);
		splay_release(rope);
	}

	TEST_SECTION("rope_insert");
	{
		t_rope	*rope = brownfox();
		// quick brown fox jumps
		// -> quick brown lazy fox jumps

		t_rope	*r01 = rope_create("l", "a");
		t_rope	*r02 = rope_create("z", "y");
		t_rope	*r03 = rope_create(" ", NULL);
		t_rope	*lazy = rope_concat(rope_concat(r01, r02), r03);

		rope = rope_insert(rope, 12, lazy);
		CHECK_EQ(rope_length(rope), 26);

		CHECK(rope);
		CHECK_EQ(rope_index(rope, 0), 'q');
		CHECK_EQ(rope_index(rope, 10), 'n');
		CHECK_EQ(rope_index(rope, 11), ' ');
		CHECK_EQ(rope_index(rope, 12), 'l');
		CHECK_EQ(rope_index(rope, 15), 'y');
		CHECK_EQ(rope_index(rope, 16), ' ');
		CHECK_EQ(rope_index(rope, 17), 'f');
		CHECK_EQ(rope_index(rope, 25), 's');
	}

	TEST_SECTION("rope_delete");
	{
		t_rope	*rope = brownfox();
		// quick brown fox jumps
		// -> quick bx jumps

		rope = rope_delete(rope, 7, 14);

		CHECK(rope);
		CHECK_EQ(rope_index(rope, 0), 'q');
		CHECK_EQ(rope_index(rope, 6), 'b');
		CHECK_EQ(rope_index(rope, 7), 'x');
		CHECK_EQ(rope_index(rope, 13), 's');
		CHECK_EQ(rope_length(rope), 14);
	}

}

int main()
{
	test_rope();

	int fail_count = print_result();
	return (fail_count);
}
