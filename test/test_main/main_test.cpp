#include <unity.h>

void setUp(void) {}
void tearDown(void) {}

void test_framework_runs(void) {
    TEST_ASSERT_TRUE(true);
}

void test_basic_arithmetic(void) {
    TEST_ASSERT_EQUAL(4, 2 + 2);
    TEST_ASSERT_EQUAL(0, 5 - 5);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_framework_runs);
    RUN_TEST(test_basic_arithmetic);
    return UNITY_END();
}
