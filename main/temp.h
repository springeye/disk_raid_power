//
// Created by develop on 2025/9/19.
//

#ifndef DISK_RAID_POWER_TEMP_H
#define DISK_RAID_POWER_TEMP_H
const int ntcPin = TEMP_PIN;
const float VREF = 1.1;       // ESP32 ADC默认参考电压为1.1V，除非你用外部VREF并已配置
const int ADC_RESOLUTION = 12; // ADC分辨率(12位)
const float TEMP_OFFSET = 20.0;
// NTC参数
const float R25 = 10000.0;    // 25°C时的电阻值(10kΩ)
const float BETA = 3380.0;     // B值(3380K)
const float R_SERIES = 10000.0; // 串联分压电阻(10kΩ)
const float MAX_POWER = 0.1;  // 100mW最大功率
const float MAX_CURRENT = 0.00031; // 310uA最大电流
const float DISSIPATION = 0.001; // 1mW/°C耗散系数
void init_temp();
float read_temp();
#endif //DISK_RAID_POWER_TEMP_H