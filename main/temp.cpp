#include "temp.h"
#include <Arduino.h>
#include <log.h>
#include <math.h>

// ---------------- 配置区域 ----------------
#define SERIES_RESISTOR 10000.0   // 实际分压电阻 (10kΩ, 请确认电路)
#define NOMINAL_RES     10000.0   // NTC 在 25℃ 的阻值 (10kΩ)
#define NOMINAL_TEMP    25.0      // 基准温度 (25℃)
#define B_COEFFICIENT   3455.0    // NTC B 值 (25℃/100℃ 区间)
// -----------------------------------------

void init_temp() {
    mylog.println("init_temp");
    analogReadResolution(12); // 0~4095
    analogSetAttenuation(ADC_11db); // 建议 0~3.3V 输入
}

float read_temp() {
    mylog.println("read_temp");
    // 直接获取毫伏数，更准确
    int mv = analogReadMilliVolts(TEMP_PIN);
    float voltage = mv / 1000.0; // 转换成伏特

    // 计算 NTC 阻值
    float resistance = SERIES_RESISTOR * voltage / (3.3 - voltage);

    // B 参数公式
    float steinhart;
    steinhart = resistance / NOMINAL_RES;          // (R/R25)
    steinhart = log(steinhart);                    // ln(R/R25)
    steinhart /= B_COEFFICIENT;                    // 1/B * ln(R/R25)
    steinhart += 1.0 / (NOMINAL_TEMP + 273.15);    // + (1/T25)
    steinhart = 1.0 / steinhart;                   // 开尔文温度
    steinhart -= 273.15;                           // 转换为摄氏度

    return steinhart;
}
