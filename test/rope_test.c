#include "test.h"
#include "../rope.h"

void	test_rope()
{
	TEST_CHAPTER("ロープ");

    TEST_SECTION("rope_create");
    {
        t_rope	*rope = rope_create();
        CHECK(rope);
        CHECK_EQ(rope->length, 0);
    }

    TEST_SECTION("rope_append");
    {
        t_rope	*rope = rope_create();

        rope = rope_append("He");
        CHECK(rope);
        CHECK_EQ(rope->length, 2);
        CHECK_EQ_STR((char*)rope->root->node, "He")
    }

    TEST_SECTION("rope_char_at");
    {
        t_rope	*rope = rope_create();

        rope = rope_append("He");
        rope = rope_append("llo,");
        rope = rope_append(" ");
        rope = rope_append("world");
        rope = rope_append("\n");
        CHECK(rope);
        CHECK_EQ(rope->length, 14);

        CHECK_EQ(rope_char_at(0), 'H');
        CHECK_EQ(rope_char_at(5), ',');
        CHECK_EQ(rope_char_at(8), 'o');
        CHECK_EQ(rope_char_at(13), '\n');
    }

    TEST_SECTION("rope_path");
    {
        t_rope	*rope = rope_create();

        rope = rope_append("He");
        rope = rope_append("ll");
        rope = rope_append("o,");
        rope = rope_append(" ");
        rope = rope_append("wo");
        rope = rope_append("r");
        rope = rope_append("ld");
        rope = rope_append("\n");

        t_splay_path	*path = rope_path(rope, 8);
        CHECK_EQ_STR((char*)path->node, "wo");
    }

}

int main()
{
	test_rope();

	int fail_count = print_result();
	return (fail_count);
}
