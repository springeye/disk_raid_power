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
    if (cell_index < 1 || cell_index > 8) return 0;

    uint8_t buf[32];

    if (cell_index <= 4) {
        // DAStatus1
        uint8_t n = read_block(0x71, buf, sizeof(buf));
        if (state_flag == 0 && n >= 8) {
            return (uint16_t)buf[(cell_index-1)*2] | ((uint16_t)buf[(cell_index-1)*2+1] << 8);
        }
    } else {
        // DAStatus2
        uint8_t n = read_block(0x72, buf, sizeof(buf));
        if (state_flag == 0 && n >= 8) {
            uint8_t idx = cell_index - 5; // 5->0, 6->1, 7->2, 8->3
            return (uint16_t)buf[idx*2] | ((uint16_t)buf[idx*2+1] << 8);
        }
    }
    return 0;
}


BQ40Z80 bq40z80;
