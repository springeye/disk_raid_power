#ifndef DISK_RAID_POWER_DATA_TYPES_H
#define DISK_RAID_POWER_DATA_TYPES_H

#include <cstdint>

struct BatteryData {
    uint8_t percent = 0;             // 电量百分比
    float voltage = 0.0f;            // 电池总电压（V）
    float current = 0.0f;            // 电池总电流（A）
    float power = 0.0f;              // 电池功率（W）
    float wh = 0.0f;                 // 电池剩余电量（Wh）
    int16_t temp = 0;                // 电池温度（°）
    float cellVoltage[6] = {0.0f};   // 6 节电芯电压（V）
    uint8_t cellCount = 6;           // 电芯数量
};

struct PortData {
    float voltage = 0.0f;            // 端口电压（V）
    float current = 0.0f;            // 端口电流（A）
    float power = 0.0f;              // 端口功率（W）
    bool isCharging = false;         // 是否充电中
    bool isDischarging = false;      // 是否放电中
};

struct SystemData {
    BatteryData battery;             // 电池数据
    PortData portC1;                 // SW6306 端口数据
    PortData portC2;                 // IP2366 端口数据
    float boardTemp = 0.0f;          // 板温（°）
    float totalInPower = 0.0f;       // 总输入功率（W）
    float totalOutPower = 0.0f;      // 总输出功率（W）
};

#endif // DISK_RAID_POWER_DATA_TYPES_H
