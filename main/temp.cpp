#include "temp.h"
#include <Arduino.h>
#include <math.h>

// ---------------- 配置区域 ----------------
#define SERIES_RESISTOR 10000.0   // 实际分压电阻 (10kΩ, 请确认电路)
#define NOMINAL_RES     10000.0   // NTC 在 25℃ 的阻值 (10kΩ)
#define NOMINAL_TEMP    25.0      // 基准温度 (25℃)
#define B_COEFFICIENT   3455.0    // NTC B 值 (25℃/100℃ 区间)
// -----------------------------------------

void init_temp() {
    analogReadResolution(12); // 0~4095
    analogSetAttenuation(ADC_11db); // 建议 0~3.3V 输入
}

float read_temp() {
    return read_temp_avg();
}

float read_temp_avg(int samples) {
    long sum_mv = 0;
    for (int i = 0; i < samples; ++i) {
        sum_mv += analogReadMilliVolts(TEMP_PIN);
        delay(2); // 可选，防止连续采样过快
    }
    float avg_mv = sum_mv / (float)samples;
    float voltage = avg_mv / 1000.0;
    float resistance = SERIES_RESISTOR * voltage / (3.3 - voltage);
    float steinhart;
    steinhart = resistance / NOMINAL_RES;
    steinhart = log(steinhart);
    steinhart /= B_COEFFICIENT;
    steinhart += 1.0 / (NOMINAL_TEMP + 273.15);
    steinhart = 1.0 / steinhart;
    steinhart -= 273.15;
    return steinhart;
}
