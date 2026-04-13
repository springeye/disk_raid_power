//
// Created by develop on 2025/9/27.
//

#include "KKPortDevice.h"

#include <bq40z80.h>
#include <ip2366.h>
#include <SW6306.h>
#include <temp.h>
#include "settings.h"

KKPortDevice::KKPortDevice(TwoWire *wire) : _wire(wire), _bq(nullptr), _sw(nullptr), _ip2366(nullptr) {
}

PortStatus KKPortDevice::buildPortStatus(bool isCharging, bool isDischarging, float voltage, float current) const {
    PortStatus status{};
    if (isCharging) {
        status.voltage = voltage;
        status.current = current;
        status.state = PortState::Input;
    } else if (isDischarging) {
        status.voltage = voltage;
        status.current = current;
        status.state = PortState::Output;
    } else {
        status.voltage = 0;
        status.current = 0;
        status.state = PortState::NONE;
    }
    return status;
}

void KKPortDevice::init() {
    _wire->begin(BQ_I2C_SDA, BQ_I2C_SCL);
    _bq = new BQ40Z80(_wire);
    _sw = new SW6306(SW6306_ADDR, _wire);
    _ip2366 = new IP2366(IP2366_INT_PIN, _wire);
    _sw->begin();
    _ip2366->begin();
}
float KKPortDevice::getPower() {
    float bq_voltage = static_cast<float>(_bq->read_voltage()) / 1000.0f;
    float bq_current = static_cast<float>(_bq->read_current()) / 1000.0f;
    float bq_power = bq_voltage * bq_current;
    return bq_power;
}

float KKPortDevice::getBatTemp() {
    return static_cast<float>(_bq->read_temp()) / 10.0f;
}

PortStatus KKPortDevice::getPortState(PortType port) {
    if (port == C1) {
        return buildPortStatus(_sw->isC1Sink(), _sw->isC1Source(), static_cast<float>(_sw->readVBUS()) / 1000.0f,
                               static_cast<float>(_sw->readIBUS()) / 1000.0f);
    } else if (port == C2) {
        return buildPortStatus(_ip2366->isCharging(), _ip2366->isDischarging(), _ip2366->getTypeCVoltage(),
                               _ip2366->getTypeCCurrent());
    }
    return PortStatus{};
}

uint8_t KKPortDevice::getPercent() {
    return _bq->read_capacity();
}

float KKPortDevice::getTotalIn() {
    PortStatus c1 = getPortState(PortType::C1);
    PortStatus c2 = getPortState(PortType::C2);
    float total_in = 0.0f;
    if (c1.state == PortState::Input) {
        total_in = c1.voltage * c1.current;
    }
    if (c2.state == PortState::Input) {
        total_in = c2.voltage * c2.current;
    }
    return total_in;
}

float KKPortDevice::getTotalOut() {
    PortStatus c1 = getPortState(PortType::C1);
    PortStatus c2 = getPortState(PortType::C2);
    float total_out = 0.0f;
    if (c1.state == PortState::Output) {
        total_out = c1.voltage * c1.current;
    }
    if (c2.state == PortState::Output) {
        total_out = c2.voltage * c2.current;
    }
    return total_out;
}

float KKPortDevice::getBoardTemp() {
    return read_temp();
}

float KKPortDevice::getCellVoltage(uint8_t index) {
    return _bq->read_cell_voltage(index);
}

float KKPortDevice::getWh(uint8_t cell_count, float cell_cutoff_v) {
    return _bq->read_remaining_energy_wh(cell_count, cell_cutoff_v);
}

float KKPortDevice::getTotalVoltage() {
    return _bq->read_voltage();
}

float KKPortDevice::getTotalCurrent() {
    return _bq->read_current();
}

void KKPortDevice::loop() {
    if (_ip2366->canCommunicate()) {
        _ip2366->readAllData();
    }
}

KKPortDevice::~KKPortDevice() {
    delete _bq;
    delete _sw;
    delete _ip2366;
    // _wire 不 delete（外部注入）
}
