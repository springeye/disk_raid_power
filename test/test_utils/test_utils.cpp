// test_utils.cpp
// 测试 extract_bits 工具函数（纯逻辑，无 LVGL/Arduino 依赖）

#include <unity.h>
#include <cstdint>

// 直接内嵌实现，避免 native 环境链接 main/ 的构建路径问题
// extract_bits: 提取 int8_t 指定区间的位（下标从 0 开始，0 为最低位）
static uint8_t extract_bits(int8_t value, uint8_t start, uint8_t end) {
    uint8_t uvalue = static_cast<uint8_t>(value);
    uint8_t mask = ((1u << (end - start + 1)) - 1u) << start;
    return (uvalue & mask) >> start;
}

void setUp(void) {}
void tearDown(void) {}

// 测试用例 1：提取最低两位（bits 0-1），值为 0b11001010 = 0xCA
void test_extract_bits_low_two_bits(void) {
    // 0xCA = 1100 1010，bits 0-1 = 10 (二进制) = 2
    uint8_t result = extract_bits((int8_t)0xCA, 0, 1);
    TEST_ASSERT_EQUAL_UINT8(0x02, result);
}

// 测试用例 2：提取中间位段（bits 2-5），值为 0b01101100 = 0x6C
void test_extract_bits_middle_range(void) {
    // 0x6C = 0110 1100，bits 2-5 = 1011 (二进制) = 11
    uint8_t result = extract_bits((int8_t)0x6C, 2, 5);
    TEST_ASSERT_EQUAL_UINT8(0x0B, result);
}

// 测试用例 3：零值输入，任意区间均应返回 0
void test_extract_bits_zero_input(void) {
    TEST_ASSERT_EQUAL_UINT8(0, extract_bits(0, 0, 7));
    TEST_ASSERT_EQUAL_UINT8(0, extract_bits(0, 3, 6));
    TEST_ASSERT_EQUAL_UINT8(0, extract_bits(0, 0, 0));
}

// 测试用例 4：提取单个 bit（start == end），验证每个 bit 的独立提取
void test_extract_bits_single_bit(void) {
    // 0x01 = 0000 0001，bit 0 = 1，其余 = 0
    TEST_ASSERT_EQUAL_UINT8(1, extract_bits((int8_t)0x01, 0, 0));
    TEST_ASSERT_EQUAL_UINT8(0, extract_bits((int8_t)0x01, 1, 1));

    // 0x80 = 1000 0000（负数），bit 7 = 1
    TEST_ASSERT_EQUAL_UINT8(1, extract_bits((int8_t)0x80, 7, 7));
    TEST_ASSERT_EQUAL_UINT8(0, extract_bits((int8_t)0x80, 0, 0));
}

// 测试用例 5：负数输入（负数按 uint8_t 解释），验证符号位处理
void test_extract_bits_negative_input(void) {
    // -1 = 0xFF = 1111 1111，任意区间均应全 1
    TEST_ASSERT_EQUAL_UINT8(0x0F, extract_bits((int8_t)-1, 0, 3));
    TEST_ASSERT_EQUAL_UINT8(0x0F, extract_bits((int8_t)-1, 4, 7));
    TEST_ASSERT_EQUAL_UINT8(0xFF, extract_bits((int8_t)-1, 0, 7));
}

// 测试用例 6：全字节提取（bits 0-7），等价于 uint8_t 强转
void test_extract_bits_full_byte(void) {
    TEST_ASSERT_EQUAL_UINT8(0xAB, extract_bits((int8_t)0xAB, 0, 7));
    TEST_ASSERT_EQUAL_UINT8(0x00, extract_bits((int8_t)0x00, 0, 7));
    TEST_ASSERT_EQUAL_UINT8(0xFF, extract_bits((int8_t)0xFF, 0, 7));
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_extract_bits_low_two_bits);
    RUN_TEST(test_extract_bits_middle_range);
    RUN_TEST(test_extract_bits_zero_input);
    RUN_TEST(test_extract_bits_single_bit);
    RUN_TEST(test_extract_bits_negative_input);
    RUN_TEST(test_extract_bits_full_byte);
    return UNITY_END();
}
