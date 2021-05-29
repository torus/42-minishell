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

		CHECK_EQ(rope->refcount, 1);

		t_splay_path	*path;
		t_splay_path	*cur;
		t_rope			*splayed;

		path = NULL;
		splayed = NULL;
		rope_index_with_path(rope, 9, &path);
		CHECK_EQ(path->refcount, 1);
		CHECK_EQ(rope->refcount, 2);
		cur = path->next;
		while (cur)
		{
			cur->node->value = ROPE_NOWEIGHT;
			cur = cur->next;
		}
		splay_assign(&splayed, splay(path->next));

		CHECK_EQ(rope_index(splayed, 0), 'q');
		CHECK_EQ(rope_index(splayed, 9), 'w');
		CHECK_EQ(rope_index(splayed, 10), 'n');
		CHECK_EQ(rope_index(splayed, 20), 's');
		CHECK_EQ(rope_length(splayed), 21);

		CHECK_EQ(rope->refcount, 2);
		splay_release(rope);
		CHECK_EQ(splayed->refcount, 1);
		splay_release(splayed);
		CHECK_EQ(path->refcount, 1);
		splay_path_release(path);
	}

	TEST_SECTION("rope_split");
	{
		t_rope	*rope;

		splay_init(&rope, brownfox());
		// quick brown fox jumps
		CHECK_EQ(rope->refcount, 1);
		CHECK(rope);
		CHECK_EQ(rope_index(rope, 0), 'q');
		CHECK_EQ(rope_index(rope, 9), 'w');
		CHECK_EQ(rope_index(rope, 10), 'n');
		CHECK_EQ(rope_index(rope, 20), 's');
		CHECK_EQ(rope_length(rope), 21);

		t_rope	*left = NULL, *right = NULL;
		rope_split(rope, 9, &left, &right);
		CHECK_EQ(left->refcount, 1);
		CHECK_EQ(right->refcount, 1);

		CHECK(left);
		CHECK_EQ(rope_length(left), 9);
		CHECK(right);
		CHECK_EQ(rope_length(right), 12);

		CHECK_EQ(rope_index(left, 0), 'q');
		CHECK_EQ(rope_index(left, 8), 'o');
		CHECK_EQ(rope_index(right, 0), 'w');
		CHECK_EQ(rope_index(right, 11), 's');

		splay_release(left);
		splay_release(right);

		CHECK_EQ(rope->refcount, 1);
		splay_release(rope);
	}

	TEST_SECTION("rope_insert 2 文字");
	{
		t_rope	*rope;
		t_rope	*result;

		splay_init(&rope,
				   rope_concat(rope_create("a", NULL),
							   rope_create("b", NULL)));
		splay_init(&result, rope_insert(rope, 1, rope_create("c", NULL)));

		CHECK(result);
		CHECK_EQ(rope_index(result, 0), 'a');
		CHECK_EQ(rope_index(result, 1), 'c');
		CHECK_EQ(rope_index(result, 2), 'b');

		splay_release(rope);
		splay_release(result);
	}

	TEST_SECTION("rope_insert");
	{
		t_rope	*rope;

		splay_init(&rope, brownfox());
		// quick brown fox jumps
		// -> quick brown lazy fox jumps

		t_rope	*r01; splay_init(&r01, rope_create("l", "a"));
		t_rope	*r02; splay_init(&r02, rope_create("z", "y"));
		t_rope	*r03; splay_init(&r03, rope_create(" ", NULL));
		t_rope	*lazy; splay_init(&lazy, rope_concat(rope_concat(r01, r02), r03));

		splay_assign(&rope, rope_insert(rope, 12, lazy));
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

		splay_release(r01);
		splay_release(r02);
		splay_release(r03);
		splay_release(lazy);
		splay_release(rope);
	}

	TEST_SECTION("rope_delete");
	{
		t_rope	*rope;

		splay_init(&rope, brownfox());
		// quick brown fox jumps
		// -> quick bx jumps

		splay_assign(&rope, rope_delete(rope, 7, 14));

		CHECK(rope);
		CHECK_EQ(rope_index(rope, 0), 'q');
		CHECK_EQ(rope_index(rope, 6), 'b');
		CHECK_EQ(rope_index(rope, 7), 'x');
		CHECK_EQ(rope_index(rope, 13), 's');
		CHECK_EQ(rope_length(rope), 14);
		splay_release(rope);
	}

	TEST_SECTION("rope_delete 最初の文字");
	{
		t_rope	*rope;

		splay_init(&rope, brownfox());
		// quick brown fox jumps
		// -> uick brown fox jumps
		//    01234567890123456789

		splay_assign(&rope, rope_delete(rope, 0, 1));

		CHECK(rope);
		CHECK_EQ(rope_index(rope, 0), 'u');
		CHECK_EQ(rope_index(rope, 1), 'i');
		CHECK_EQ(rope_index(rope, 19), 's');
		CHECK_EQ(rope_length(rope), 20);
		splay_release(rope);
	}

	TEST_SECTION("rope_delete 最初の 3 文字");
	{
		t_rope	*rope;

		splay_init(&rope, brownfox());
		// quick brown fox jumps
		// -> ck brown fox jumps
		//    012345678901234567

		splay_assign(&rope, rope_delete(rope, 0, 3));

		CHECK(rope);
		CHECK_EQ(rope_index(rope, 0), 'c');
		CHECK_EQ(rope_index(rope, 1), 'k');
		CHECK_EQ(rope_index(rope, 17), 's');
		CHECK_EQ(rope_length(rope), 18);
		splay_release(rope);
	}

	TEST_SECTION("rope_delete 最後の文字");
	{
		t_rope	*rope;

		splay_init(&rope, brownfox());
		// quick brown fox jumps
		// -> quick brown fox jump
		//    01234567890123456789

		splay_assign(&rope, rope_delete(rope, 20, 21));

		CHECK(rope);
		CHECK_EQ(rope_index(rope, 0), 'q');
		CHECK_EQ(rope_index(rope, 1), 'u');
		CHECK_EQ(rope_index(rope, 19), 'p');
		CHECK_EQ(rope_length(rope), 20);
		splay_release(rope);
	}

	TEST_SECTION("rope_delete 最後の 3 文字");
	{
		t_rope	*rope;

		splay_init(&rope, brownfox());
		// quick brown fox jumps
		// -> quick brown fox ju
		//    012345678901234567

		splay_assign(&rope, rope_delete(rope, 18, 21));

		CHECK(rope);
		CHECK_EQ(rope_index(rope, 0), 'q');
		CHECK_EQ(rope_index(rope, 1), 'u');
		CHECK_EQ(rope_index(rope, 16), 'j');
		CHECK_EQ(rope_index(rope, 17), 'u');
		CHECK_EQ(rope_length(rope), 18);
		splay_release(rope);
	}

	TEST_SECTION("rope_delete 全部消す");
	{
		t_rope	*rope;

		splay_init(&rope, rope_create("1", NULL));

		CHECK_EQ(rope_length(rope), 1);

		splay_assign(&rope, rope_delete(rope, 0, 1));

		CHECK(!rope);
		CHECK_EQ(rope_length(rope), 0);
		splay_release(rope);
	}

}

int main()
{
	test_rope();

	int fail_count = print_result();
	return (fail_count);
}
