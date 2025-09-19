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
float read_temp()
{
    int adcValue = analogRead(ntcPin);
    float voltage = adcValue * (VREF / 4095.0); // 4095 = 2^12 - 1

    // 计算NTC当前电阻值
    float ntcResistance = R_SERIES * (VREF / voltage - 1.0);

    // 使用Steinhart-Hart方程计算温度
    float steinhart;
    steinhart = ntcResistance / R25;          // (R/Ro)
    steinhart = log(steinhart);               // ln(R/Ro)
    steinhart /= BETA;                        // 1/B * ln(R/Ro)
    steinhart += 1.0 / (25.0 + 273.15);       // + (1/To)
    steinhart = 1.0 / steinhart;              // 倒数
    float tempC = steinhart - 273.15;         // 转换为摄氏度

    mylog.printf("ADC: %d, Voltage: %.2fV, R_ntc: %.2fΩ, Temp: %.2f°C\n",
                 adcValue, voltage, ntcResistance, tempC);
    return tempC;
}