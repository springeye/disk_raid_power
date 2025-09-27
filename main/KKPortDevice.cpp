//
// Created by develop on 2025/9/27.
//

#include "KKPortDevice.h"

#include <bq40z80.h>
#include <ip2366.h>
#include <SW6306.h>
#include <temp.h>
// 声明独立的 TwoWire 实例
TwoWire wire1(1); // 1为I2C总线编号，ESP32等平台支持多个I2C实例
BQ40Z80* bq = nullptr;
SW6306*  sw = nullptr;
IP2366*  ip2366 = nullptr;
void KKPortDevice::init()
{
    wire1.begin(26, 25);
    bq = new BQ40Z80(&wire1);
    sw = new SW6306(0x3C, &wire1);
    ip2366 = new IP2366( 14,&wire1);
    sw->begin();
    ip2366->begin();
}
float KKPortDevice::getPower()
{
    float bq_voltage = static_cast<float>(bq->read_voltage())/1000.0f;
    float bq_current = static_cast<float>(bq->read_current())/1000.0f;
    float bq_power = bq_voltage*bq_current;
    return bq_power;
}

float KKPortDevice::getBatTemp()
{
    return static_cast<float>(bq->read_temp())/10.0f;
}

PortStatus KKPortDevice::getPortState(PortType port)
{
    if (port==C1)
    {

        bool is6306DisCharging=sw->isC1Source();
        bool is6306Charging=sw->isC1Sink();
        PortStatus status{};
        if (is6306Charging)
        {
            status.voltage=static_cast<float>(sw->readVBUS())/1000.0f;
            status.current=static_cast<float>(sw->readIBUS())/1000.0f;
            status.state=PortState::Input;
        }else if (is6306DisCharging)
        {
            status.voltage=static_cast<float>(sw->readVBUS())/1000.0f;
            status.current=static_cast<float>(sw->readIBUS())/1000.0f;
            status.state=PortState::Output;
        }else
        {
            status.voltage=0;
            status.current=0;
            status.state=PortState::NONE;
        }

        return status;

    }else if (port==C2)
    {

        bool is2366DisCharging=ip2366->isDischarging();
        bool is2366Charging=ip2366->isCharging();
        PortStatus status{};
        if (is2366Charging)
        {
            status.voltage=ip2366->getTypeCVoltage();
            status.current=ip2366->getTypeCCurrent();
            status.state=PortState::Input;
        }else if (is2366DisCharging)
        {
            status.voltage=ip2366->getTypeCVoltage();
            status.current=ip2366->getTypeCCurrent();
            status.state=PortState::Output;
        }else
        {
            status.voltage=0;
            status.current=0;
            status.state=PortState::NONE;
        }

        return status;
    }
    // 直接返回默认值，避免调用纯虚函数
    return PortStatus{};
}

uint8_t KKPortDevice::getPercent()
{
    return bq->read_capacity();
}

float KKPortDevice::getTotalIn()
{
    PortStatus c1 = getPortState(PortType::C1);
    PortStatus c2 = getPortState(PortType::C2);
    float total_in=0.0f;
    if (c1.state==PortState::Input)
    {
        total_in=c1.voltage*c1.current;
    }
    if (c2.state==PortState::Input)
    {
        total_in=c2.voltage*c2.current;
    }
    return total_in;
}

float KKPortDevice::getTotalOut()
{
    PortStatus c1 = getPortState(PortType::C1);
    PortStatus c2 = getPortState(PortType::C2);
    float total_out=0.0f;
    if (c1.state==PortState::Output)
    {
        total_out=c1.voltage*c1.current;
    }
    if (c2.state==PortState::Output)
    {
        total_out=c2.voltage*c2.current;
    }
    return total_out;
}

float KKPortDevice::getBoardTemp()
{
    return read_temp();
}

float KKPortDevice::getCellVoltage(uint8_t index)
{
    return bq->read_cell_voltage(index);
}

float KKPortDevice::getWh(uint8_t cell_count, float cell_cutoff_v)
{
    return bq->read_remaining_energy_wh(cell_count,cell_cutoff_v);
}

float KKPortDevice::getTotalVoltage()
{
    return bq->read_voltage();
}

float KKPortDevice::getTotalCurrent()
{
    return bq->read_current();
}

void KKPortDevice::loop()
{
    if (ip2366->canCommunicate()) {
        ip2366->readAllData();
    }
}

KKPortDevice::~KKPortDevice()
= default;
