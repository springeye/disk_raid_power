//
// Created by develop on 2025/9/27.
//

#include "ESMPortDevice.h"

#include <bq40z80.h>
#include <i2c_utils.h>
#include <INA226_WE.h>
#include <log.h>
#define INA226_XT_ADDR 0x88
#define INA226_AC_ADDR 0x80
#define INA226_140_ADDR 0x82
// 声明独立的 TwoWire 实例
TwoWire wire226(1); // 1为I2C总线编号，ESP32等平台支持多个I2C实例
TwoWire wirebq(2); // 1为I2C总线编号，ESP32等平台支持多个I2C实例
INA226_WE* inaXT= nullptr;
INA226_WE* inaAC= nullptr;
INA226_WE* ina140= nullptr;
BQ40Z80* bq40z80;
void ESMPortDevice::init()
{
    wire226.begin(10, 11);
    wirebq.begin(17,16);
    list_i2c_devices(wirebq, 2);
    bq40z80=new BQ40Z80(&wirebq);
    inaXT = new INA226_WE(&wire226, INA226_XT_ADDR);
    inaAC = new INA226_WE(&wire226, INA226_AC_ADDR);
    ina140 = new INA226_WE(&wire226, INA226_140_ADDR);
    inaXT->init();
    inaAC->init();
    ina140->init();
    inaXT->setAverage(AVERAGE_16);
    inaXT->setConversionTime(CONV_TIME_1100, CONV_TIME_1100);
    inaXT->setResistorRange(0.001,10.0);
    inaAC->setAverage(AVERAGE_16);
    inaAC->setConversionTime(CONV_TIME_1100, CONV_TIME_1100);
    inaAC->setResistorRange(0.005,10.0);
    ina140->setAverage(AVERAGE_16);
    ina140->setConversionTime(CONV_TIME_1100, CONV_TIME_1100);
    ina140->setResistorRange(0.005,10.0);
}
float ESMPortDevice::getPower()
{
    float bq_voltage = static_cast<float>(bq40z80->read_voltage())/1000.0f;
    float bq_current = static_cast<float>(bq40z80->read_current())/1000.0f;
    float bq_power = bq_voltage*bq_current;
    return bq_power;
}

float ESMPortDevice::getBatTemp()
{
    return static_cast<float>(bq40z80->read_temp())/10.0f;
}

PortStatus ESMPortDevice::getPortState(PortType port)
{

    return PortStatus{};
}

uint8_t ESMPortDevice::getPercent()
{
    return bq40z80->read_capacity();
}

float ESMPortDevice::getTotalIn()
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

float ESMPortDevice::getTotalOut()
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

float ESMPortDevice::getBoardTemp()
{
    return 0.0f;
}

float ESMPortDevice::getCellVoltage(uint8_t index)
{
    return bq40z80->read_cell_voltage(index);
}

float ESMPortDevice::getWh(uint8_t cell_count, float cell_cutoff_v)
{
    return bq40z80->read_remaining_energy_wh(cell_count,cell_cutoff_v);
}

float ESMPortDevice::getTotalVoltage()
{
    return bq40z80->read_voltage();
}

float ESMPortDevice::getTotalCurrent()
{
    return bq40z80->read_current();
}

void ESMPortDevice::loop()
{
    // float shuntVoltage_mV = 0.0;
    // float loadVoltage_V = 0.0;
    // float busVoltage_V = 0.0;
    // float current_mA = 0.0;
    // float power_mW = 0.0;
    //
    // inaAC->readAndClearFlags();
    // shuntVoltage_mV = inaAC->getShuntVoltage_mV();
    // busVoltage_V = inaAC->getBusVoltage_V();
    // current_mA = inaAC->getCurrent_mA();
    // power_mW = inaAC->getBusPower();
    // loadVoltage_V  = busVoltage_V + (shuntVoltage_mV/1000);
    //
    // mylog.print("Shunt Voltage [mV]: "); mylog.println(shuntVoltage_mV);
    // mylog.print("Bus Voltage [V]: "); mylog.println(busVoltage_V);
    // mylog.print("Load Voltage [V]: "); mylog.println(loadVoltage_V);
    // mylog.print("Current[mA]: "); mylog.println(current_mA);
    // mylog.print("Bus Power [mW]: "); mylog.println(power_mW);
    // if(!inaAC->overflow){
    //     mylog.println("Values OK - no overflow");
    // }
    // else{
    //     mylog.println("Overflow! Choose higher current range");
    // }
}

ESMPortDevice::~ESMPortDevice()
= default;

