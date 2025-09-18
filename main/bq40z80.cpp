#include "bq40z80.h"

#include <log.h>


BQ40Z80::BQ40Z80()
{
    state_flag = 0;
    memset(word_buf, 0, sizeof(word_buf));
}


uint8_t BQ40Z80::calculate_crc8(uint8_t InitialValue, uint8_t* message, uint8_t len)
{
    uint8_t i, crc = InitialValue;
    while (len--)
    {
        crc ^= (*message++);
        for (i = 0; i < 8; i++)
        {
            if (crc & 0x80)
            {
                crc = (crc << 1) ^ 0x07;
            }
            else
            {
                crc <<= 1;
            }
        }
    }
    return crc;
}

void BQ40Z80::read_word(uint8_t memory_addr)
{
    word_buf[0] = 0;
    word_buf[1] = 0;
    word_buf[2] = 0;
    word_buf[3] = 0;
    word_buf[4] = 0;
    word_buf[5] = 0;

    // 1. 发送命令字
    Wire.beginTransmission(BQ40Z80_ADDR);
    Wire.write(memory_addr);
    if (Wire.endTransmission(false) != 0)
    {
        state_flag = 1; // 通讯连接错误
        return;
    }

    // 2. 请求 3 字节：LSB, MSB, CRC
    uint8_t count = Wire.requestFrom(BQ40Z80_ADDR, (uint8_t)3);
    if (count != 3)
    {
        state_flag = 1; // 通讯错误
        return;
    }

    word_buf[3] = Wire.read(); // LSB
    word_buf[4] = Wire.read(); // MSB
    word_buf[5] = Wire.read(); // CRC

    // 3. 构造 CRC 校验数组：SLA+W, CMD, SLA+R, LSB, MSB
    uint8_t crc_buf[5];
    crc_buf[0] = (BQ40Z80_ADDR << 1) | 0; // SLA+W
    crc_buf[1] = memory_addr;             // 命令
    crc_buf[2] = (BQ40Z80_ADDR << 1) | 1; // SLA+R
    crc_buf[3] = word_buf[3];             // LSB
    crc_buf[4] = word_buf[4];             // MSB

    // 4. 计算 CRC 并比较
    if (calculate_crc8(0x00, crc_buf, 5) == word_buf[5])
    {
        state_flag = 0; // CRC 校验通过
    }
    else
    {
        state_flag = 2; // CRC 校验失败
        // 调试期间可以打印 CRC
        mylog.print("CRC fail, calc=");
        mylog.print(calculate_crc8(0x00, crc_buf, 5), HEX);
        mylog.print(", read=");
        mylog.println(word_buf[5], HEX);
    }
}

int16_t BQ40Z80::read_temp()
{
    read_word(0x08);
    if (state_flag == 0)
    {
        return ((int16_t)(word_buf[4] << 8 | word_buf[3]) - 2731);
    }
    else
    {
        return 0;
    }
}

uint16_t BQ40Z80::read_voltage()
{
    read_word(0x09);
    if (state_flag == 0)
    {
        return word_buf[4] << 8 | word_buf[3];
    }
    else
    {
        return 0;
    }
}

int16_t BQ40Z80::read_current()
{
    read_word(0x0A);
    if (state_flag == 0)
    {
        return (int16_t)(word_buf[4] << 8 | word_buf[3]);
    }
    else
    {
        return 0;
    }
}

uint8_t BQ40Z80::read_capacity()
{
    read_word(0x0D);
    if (state_flag == 0)
    {
        return word_buf[3];
    }
    else
    {
        return 0;
    }
}

uint16_t BQ40Z80::read_Remaining_Capacity()
{
    read_word(0x0F);
    if (state_flag == 0)
    {
        return word_buf[4] << 8 | word_buf[3];
    }
    else
    {
        return 0;
    }
}

uint16_t BQ40Z80::read_FullChargeCapacity()
{
    read_word(0x10);
    if (state_flag == 0)
    {
        return word_buf[4] << 8 | word_buf[3];
    }
    else
    {
        return 0;
    }
}

uint16_t BQ40Z80::read_RunTimeToEmpty()
{
    read_word(0x11);
    if (state_flag == 0)
    {
        return word_buf[4] << 8 | word_buf[3];
    }
    else
    {
        return 0;
    }
}

uint16_t BQ40Z80::read_AverageTimeToEmpty()
{
    read_word(0x12);
    if (state_flag == 0)
    {
        return word_buf[4] << 8 | word_buf[3];
    }
    else
    {
        return 0;
    }
}

uint16_t BQ40Z80::read_AverageTimeToFull()
{
    read_word(0x13);
    if (state_flag == 0)
    {
        return word_buf[4] << 8 | word_buf[3];
    }
    else
    {
        return 0;
    }
}

uint16_t BQ40Z80::read_ChargingCurrent()
{
    read_word(0x14);
    if (state_flag == 0)
    {
        return word_buf[4] << 8 | word_buf[3];
    }
    else
    {
        return 0;
    }
}

uint16_t BQ40Z80::read_ChargingVoltage()
{
    read_word(0x15);
    if (state_flag == 0)
    {
        return word_buf[4] << 8 | word_buf[3];
    }
    else
    {
        return 0;
    }
}

uint16_t BQ40Z80::read_CycleCount()
{
    read_word(0x17);
    if (state_flag == 0)
    {
        return word_buf[4] << 8 | word_buf[3];
    }
    else
    {
        return 0;
    }
}

uint8_t BQ40Z80::read_RelativeStateOfCharge()
{
    read_word(0x4F);
    if (state_flag == 0)
    {
        return word_buf[3];
    }
    else
    {
        return 0;
    }
}
//ManufacturerAccess方式读取更底层的寄存器
uint8_t BQ40Z80::read_block(uint8_t command, uint8_t *buf, uint8_t len)
{
    Wire.beginTransmission(BQ40Z80_ADDR);
    Wire.write(command);
    if (Wire.endTransmission(false) != 0)
    {
        state_flag = 1;
        return 0;
    }

    // Block Read: 第一个字节是长度
    uint8_t count = Wire.requestFrom(BQ40Z80_ADDR, len + 1);
    if (count < 1) {
        state_flag = 2;
        return 0;
    }

    uint8_t blockLen = Wire.read(); // 第一个字节是数据长度
    if (blockLen > len) blockLen = len;

    for (uint8_t i = 0; i < blockLen; i++) {
        if (Wire.available())
            buf[i] = Wire.read();
    }

    state_flag = 0;
    return blockLen;
}

/**
 *
 * @param cell_index 1-8(电芯编号)
 * @return
 */
uint16_t BQ40Z80::read_cell_voltage(uint8_t cell_index)
{
    if (cell_index < 1 || cell_index > 7) return 0;

    if (cell_index <= 3) {
        uint8_t buf[8];
        uint8_t n = read_block(0x71, buf, sizeof(buf));
        if (state_flag == 0 && n >= 6) {
            return (uint16_t)buf[(cell_index-1)*2] | ((uint16_t)buf[(cell_index-1)*2+1] << 8);
        }
    } else {
        // cell4~cell7分别对应0x3F~0x3C
        uint8_t cell_addr = 0x3F - (cell_index - 4);
        read_word(cell_addr);
        if (state_flag == 0) {
            return (uint16_t)(word_buf[4] << 8 | word_buf[3]);
        }
    }
    return 0;
}
uint16_t BQ40Z80::read_battery_status()
{
    read_word(0x16); // BatteryStatus
    if (state_flag == 0)
    {
        return (word_buf[4] << 8) | word_buf[3];
    }
    return 0;
}

bool BQ40Z80::is_charging()
{
    uint16_t status = read_battery_status();
    return (status & (1 << 11)) != 0; // CHARGING 位
}

bool BQ40Z80::is_discharging()
{
    uint16_t status = read_battery_status();
    return (status & (1 << 10)) != 0; // DISCHARGING 位
}
float BQ40Z80::read_remaining_energy_wh(uint8_t cell_count = 6, float cell_cutoff_v = 3.0f)
{
    uint16_t remaining_mAh = read_Remaining_Capacity(); // mAh
    uint16_t pack_mV = read_voltage();                 // mV

    // 转换单位
    float remaining_Ah = remaining_mAh / 1000.0f;
    float pack_V = pack_mV / 1000.0f;

    // 放电截止电压总和
    float cutoff_V = cell_cutoff_v * cell_count;

    // 近似平均电压（线性估算）
    float avg_V = cutoff_V + (pack_V - cutoff_V) / 2.0f;

    // 剩余能量
    float energy_Wh = remaining_Ah * avg_V;

    return energy_Wh;
}


// C API 实现
#include "bq40z80_c_api.h"

extern "C" {
    uint8_t bq_get_percent(void) {
        return bq.read_RelativeStateOfCharge();
    }
    uint16_t bq_get_cell_voltage(uint8_t cell_index) {
        return bq.read_cell_voltage(cell_index);
    }
    float read_remaining_energy_wh(uint8_t cell_count, float cell_cutoff_v)
    {
        return bq.read_remaining_energy_wh(cell_count, cell_cutoff_v);
    }
    float read_voltage()
    {
        return bq.read_voltage();
    }
}

BQ40Z80 bq40z80;

