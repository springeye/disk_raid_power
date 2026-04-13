// test_cell_helper.cpp
// 测试 cell_helper 的纯逻辑部分（不依赖 LVGL 运行时）
//
// cell_helper.cpp 中 lv_label_set_text_float / init_cells / update_cells
// 均依赖 LVGL 运行时对象（lv_obj_t*、lv_label_set_text 等），
// 无法在 native 环境直接调用。
//
// 本文件测试其核心格式化逻辑的等价纯 C 实现，
// 验证 snprintf 字符串格式化行为符合预期（即 lv_label_set_text_float 实际产生的输出）。

#include <unity.h>
#include <cstdio>
#include <cstring>
#include <cstdint>

void setUp(void) {}
void tearDown(void) {}

// 等价于 cell_helper.cpp 中 lv_label_set_text_float 的格式化逻辑（不调用 LVGL）
// 使用 snprintf 替代 dtostrf（dtostrf 是 Arduino 专用函数，native 环境无此函数）
static void format_float_label(char *out, size_t out_size,
                                const char *fmt, float val, int decimals) {
    char buf[32];
    snprintf(buf, sizeof(buf), "%.*f", decimals, val);
    snprintf(out, out_size, fmt, buf);
}

// 测试用例 1：正常电压值（3 位小数），格式化为 "%s"
void test_format_float_label_normal_voltage(void) {
    char out[64];
    format_float_label(out, sizeof(out), "%s", 3.750f, 3);
    TEST_ASSERT_EQUAL_STRING("3.750", out);
}

// 测试用例 2：零电压，3 位小数
void test_format_float_label_zero_voltage(void) {
    char out[64];
    format_float_label(out, sizeof(out), "%s", 0.0f, 3);
    TEST_ASSERT_EQUAL_STRING("0.000", out);
}

// 测试用例 3：最大单节锂电池电压（约 4.2V），3 位小数
void test_format_float_label_max_voltage(void) {
    char out[64];
    format_float_label(out, sizeof(out), "%s", 4.2f, 3);
    // 4.200 精度问题：snprintf 实际会输出 "4.200"
    TEST_ASSERT_EQUAL_STRING("4.200", out);
}

// 测试用例 4：带前缀格式字符串（如 "V:%s"）
void test_format_float_label_with_prefix(void) {
    char out[64];
    format_float_label(out, sizeof(out), "V:%s", 3.6f, 2);
    TEST_ASSERT_EQUAL_STRING("V:3.60", out);
}

// 测试用例 5：小数位数为 0（整数显示）
void test_format_float_label_zero_decimals(void) {
    char out[64];
    format_float_label(out, sizeof(out), "%s", 3.9f, 0);
    TEST_ASSERT_EQUAL_STRING("4", out);  // snprintf 四舍五入
}

// 测试用例 6：output buffer 足够大，不发生截断
void test_format_float_label_buffer_not_truncated(void) {
    char out[64];
    format_float_label(out, sizeof(out), "%s", 3.123f, 3);
    // 验证字符串长度合理（5 字符：3.123）
    TEST_ASSERT_EQUAL_INT(5, (int)strlen(out));
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_format_float_label_normal_voltage);
    RUN_TEST(test_format_float_label_zero_voltage);
    RUN_TEST(test_format_float_label_max_voltage);
    RUN_TEST(test_format_float_label_with_prefix);
    RUN_TEST(test_format_float_label_zero_decimals);
    RUN_TEST(test_format_float_label_buffer_not_truncated);
    return UNITY_END();
}
