//
// Created by develop on 2025/9/19.
//

#include "temp.h"
#include <Arduino.h>
#include "log.h"
void init_temp()
{
    analogReadResolution(ADC_RESOLUTION);
}
float calculateTemperature(int adcValue) {
    // 计算电压
    float voltage = adcValue * (VREF / 4095.0);

    // 防止除零错误和边界情况
    if (voltage < 0.01) voltage = 0.01;
    if (voltage > VREF - 0.01) voltage = VREF - 0.01;

    // 计算NTC当前电阻值
    float ntcResistance = R_SERIES * (VREF / voltage - 1.0);

    // 使用Steinhart-Hart方程计算温度（开尔文）
    float tempK = 1.0 / (1.0/298.15 + 1.0/BETA * log(ntcResistance/R25));
    float tempC = tempK - 273.15;

    return tempC+ TEMP_OFFSET;
}
// 检查NTC是否在安全工作范围内
void checkSafety(float tempC, float ntcResistance) {
    float current = VREF / (R_SERIES + ntcResistance);
    float power = current * current * ntcResistance;

    if (power > MAX_POWER) {
        mylog.printf("警告: 功率超过限制! %.2fmW > %.2fmW\n", power*1000, MAX_POWER*1000);
    }
    if (current > MAX_CURRENT) {
        mylog.printf("警告: 电流超过限制! %.2fuA > %.2fuA\n", current*1000000, MAX_CURRENT*1000000);
    }
    if (tempC < -40 || tempC > 125) {
        mylog.println("警告: 温度超出工作范围!");
    }
}

float read_temp()
{
    int adcValue = analogRead(ntcPin);
    float voltage = adcValue * (VREF / 4095.0);
    float ntcResistance = R_SERIES * (VREF / voltage - 1.0);
    float temperature = calculateTemperature(adcValue);

    // 显示完整信息
    mylog.println("===================================");
    mylog.printf("ADC值: %d\n", adcValue);
    mylog.printf("电压: %.3fV\n", voltage);
    mylog.printf("NTC电阻: %.1fΩ\n", ntcResistance);
    mylog.printf("温度: %.2f°C\n", temperature);

    // 安全检查
    checkSafety(temperature, ntcResistance);
    return temperature;
}