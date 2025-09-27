//
// Created by develop on 2025/9/27.
//

#include "KKPortDevice.h"

#include <bq40z80.h>
#include <ip2366.h>
#include <SW6306.h>
#include <temp.h>
// 声明独立的 TwoWire 实例
TwoWire bqWire(1); // 1为I2C总线编号，ESP32等平台支持多个I2C实例
BQ40Z80* bq = nullptr;
SW6306 sw;
IP2366 ip2366;
void KKPortDevice::init()
{
    bqWire.begin(26, 25);
    bq = new BQ40Z80(&bqWire);
}
int16_t KKPortDevice::getBatTemp()
{
    return bq->read_temp();
}

PortStatus KKPortDevice::getPortState(PortType port)
{
    return IPortDevice::getPortState(port);
}

uint8_t KKPortDevice::getPercent()
{
    return bq->read_capacity();
}

float KKPortDevice::getTotalIn()
{
}

float KKPortDevice::getTotalOut()
{
}

int16_t KKPortDevice::getBoardTemp()
{
    return read_temp();
}

float KKPortDevice::getCellVoltage(uint8_t index)
{
    return bq->read_cell_voltage(index);
}

KKPortDevice::~KKPortDevice()
{
}
