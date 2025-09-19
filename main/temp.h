//
// Created by develop on 2025/9/19.
//

#ifndef DISK_RAID_POWER_TEMP_H
#define DISK_RAID_POWER_TEMP_H
const int ntcPin = TEMP_PIN;
const float VREF = 3.3;       // ESP32 ADC参考电压
const int ADC_RESOLUTION = 12; // ADC分辨率(12位)

// NTC参数
const float R25 = 10000.0;    // 25°C时的电阻值(10kΩ)
const float BETA = 3380.0;     // B值(3380K)
const float R_SERIES = 10000.0; // 串联分压电阻(10kΩ)
void init_temp();
float read_temp();
#endif //DISK_RAID_POWER_TEMP_H