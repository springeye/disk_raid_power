//
// Created by develop on 2025/9/20.
//

#include "utils.h"

// 静态方法：提取int8_t指定区间的位（下标从0开始，0为最低位）
static uint8_t extract_bits(int8_t value, uint8_t start, uint8_t end){
    uint8_t uvalue = static_cast<uint8_t>(value);
    uint8_t mask = ((1u << (end - start + 1)) - 1u) << start;
    return (uvalue & mask) >> start;
}