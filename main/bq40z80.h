#ifndef __BQ40Z80_H
#define __BQ40Z80_H
#include <Arduino.h>
#include <Wire.h>


class BQ40Z80 {
public:
    uint8_t state_flag; // 状态标志位
    uint8_t word_buf[6];
    TwoWire* wire; // 新增成员变量，保存I2C对象

    BQ40Z80(TwoWire* wire); // 构造函数传入I2C对象

    int16_t read_temp(); // 读取电池温度 单位0.1℃
    uint16_t read_voltage(); // 读取电池组电压 单位：mv
    int16_t read_current(); // 读取电池电流 单位：ma
    uint8_t read_capacity(); // 读取电池电量 单位：%
    uint16_t read_Remaining_Capacity(); // 读取电池剩余容量 单位：mah
    uint16_t read_FullChargeCapacity();//电池完全充电时的预测容量。返回的值不会是 充电期间更新。
    uint16_t read_RunTimeToEmpty(); // 读取剩余放电时间 单位：min
    uint16_t read_AverageTimeToEmpty(); // 平均放电时间 单位：min
    uint16_t read_AverageTimeToFull(); // 读取剩余充电时间 单位：min
    uint16_t read_ChargingCurrent(); // 获取电池支持的充电电流
    uint16_t read_ChargingVoltage(); // 获取电池支持的充电电压
    uint16_t read_CycleCount(); // 读取电池循环次数
    uint8_t read_RelativeStateOfCharge(); // 读取电池健康值 单位：%
    uint8_t read_block(uint8_t command, uint8_t* buf, uint8_t len);
    uint16_t read_cell_voltage(uint8_t cell_index); // 读取单体电池电压 单位：mv cell_index:1~6
    /**
    15	FULLY_CHARGED	电池满电
    14	FULLY_DISCHARGED	电池放空
    13	TERMINATE_CHARGE	充电终止条件
    12	TERMINATE_DISCHARGE	放电终止条件
    11	CHARGING	电池正在充电
    10	DISCHARGING	电池正在放电
    9	SOC_ALERT	SOC 警告
    8	TIME_TO_EMPTY_ALERT	放电时间警告
     */
    uint16_t read_battery_status();//读取电池状态
    bool is_charging();
    bool is_discharging();
    float read_remaining_energy_wh(uint8_t cell_count, float cell_cutoff_v);

private:
    uint8_t calculate_crc8(uint8_t InitialValue, uint8_t *message, uint8_t len);
    void read_word(uint8_t memory_addr);
};


#endif
