//
// Created by develop on 2025/9/19.
//

#include "monitor_api.h"
#include "ui_presenter.h"
#include "data_types.h"
#include <log.h>
#include <temp.h>
#include <cmath>

static IPortDevice* device = nullptr;

void monitor_api_set_device(IPortDevice* dev) {
    device = dev;
}
IPortDevice* monitor_api_get_device() {
    return device;
}

// 采集电池数据
static void collectBatteryData(BatteryData& bd) {
    bd.percent    = device->getPercent();
    bd.voltage    = device->getTotalVoltage() / 1000.0f;
    bd.current    = device->getTotalCurrent() / 1000.0f;
    bd.power      = device->getPower();
    bd.wh         = device->getWh(6, 3.0f);
    bd.temp       = static_cast<int16_t>(device->getBatTemp());
    bd.cellCount  = 6;
    for (int i = 0; i < 6; ++i) {
        bd.cellVoltage[i] = device->getCellVoltage(i + 1) / 1000.0f;
    }
}

// 采集端口数据
static void collectPortData(PortData& pd, PortType type) {
    auto s = device->getPortState(type);
    pd.isCharging    = (s.state == PortState::Input);
    pd.isDischarging = (s.state == PortState::Output);
    if (!pd.isCharging && !pd.isDischarging) {
        pd.voltage = 0.0f;
        pd.current = 0.0f;
        pd.power   = 0.0f;
    } else {
        pd.voltage = s.voltage;
        pd.current = std::fabs(s.current);
        pd.power   = pd.voltage * pd.current;
    }
}

extern "C" {
    uint8_t battery_get_percent(void) {
        return device->getPercent();
    }
    uint16_t battery_get_cell_voltage(uint8_t cell_index) {
        return device->getCellVoltage(cell_index);
    }
    float battery_get_remaining_energy_wh(uint8_t cell_count, float cell_cutoff_v)
    {
        return device->getWh(cell_count, cell_cutoff_v);
    }
    float battery_get_voltage()
    {
        return device->getTotalVoltage();
    }
    float battery_get_current()
    {
        return device->getTotalCurrent();
    }
    float battery_get_power()
    {

        return device->getPower();
    }
    int16_t battery_get_temp()
    {
        return device->getBatTemp();
    }
}
extern "C" {
    float port_c2_get_voltage()
    {
        return device->getPortState(PortType::C2).voltage;
    }
    float port_c2_get_current()
    {
        return device->getPortState(PortType::C2).current;
    }
    float port_c2_get_power()
    {
        auto status = device->getPortState(PortType::C2);
        return status.voltage*status.current;
    }
    bool port_c2_is_charging()
    {
        return device->getPortState(PortType::C2).state==PortState::Input;
    }
    bool port_c2_is_discharging()
    {
        return device->getPortState(PortType::C2).state==PortState::Output;
    }
}


extern "C" {
void updateUI()
{
    device->loop();

    SystemData data;

    // 采集电池数据
    collectBatteryData(data.battery);

    // 采集端口数据
    collectPortData(data.portC1, PortType::C1);  // SW6306
    collectPortData(data.portC2, PortType::C2);  // IP2366

    // 板温
    data.boardTemp = device->getBoardTemp();

    // 计算总输入/输出功率
    data.totalInPower  = 0.0f;
    data.totalOutPower = 0.0f;
    if (data.portC1.isCharging)    data.totalInPower  += data.portC1.power;
    if (data.portC2.isCharging)    data.totalInPower  += data.portC2.power;
    if (data.portC1.isDischarging) data.totalOutPower += data.portC1.power;
    if (data.portC2.isDischarging) data.totalOutPower += data.portC2.power;

    // 委托 UI 展示
    ui_present_all(&data);
}
}
