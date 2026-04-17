//
// settings.h — 集中配置头文件
//
// 所有硬编码常量均在此定义，每个宏使用 #ifndef 保护，
// 允许通过 PlatformIO build_flags (-D<MACRO>=<value>) 覆盖。
// =============================================================
#ifndef DISK_RAID_POWER_SETTINGS_H
#define DISK_RAID_POWER_SETTINGS_H

// -------------------------------------------------------------
// GPIO 引脚配置
// -------------------------------------------------------------
// 电源控制引脚（拉低触发节能/关机）
#ifndef PIN_POWER_CTRL
#define PIN_POWER_CTRL 12
#endif

// 温度传感器引脚（板级 NTC 温度传感器，通道编号）
#ifndef TEMP_PIN
#define TEMP_PIN 36
#endif

// -------------------------------------------------------------
// I2C 总线配置（wire1，编号 1）
// -------------------------------------------------------------
// SDA 引脚
#ifndef BQ_I2C_SDA
#define BQ_I2C_SDA 26
#endif

// SCL 引脚
#ifndef BQ_I2C_SCL
#define BQ_I2C_SCL 25
#endif

// IP2366 中断引脚（INT 输入）
#ifndef IP2366_INT_PIN
#define IP2366_INT_PIN 14
#endif

// -------------------------------------------------------------
// I2C 设备地址（platformio.ini build_flags 中已有覆盖，此处提供默认值）
// -------------------------------------------------------------
#ifndef BQ40Z80_ADDR
#define BQ40Z80_ADDR 0x0B
#endif

#ifndef IP2366_ADDR
#define IP2366_ADDR 0x75
#endif

#ifndef SW6306_ADDR
#define SW6306_ADDR 0x3C
#endif

// -------------------------------------------------------------
// WiFi / AP 配置
// -------------------------------------------------------------
// AP 热点 SSID
#ifndef WIFI_SSID
#define WIFI_SSID "disk_raid_power"
#endif

// AP 热点密码（最少 8 位）
#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD "12345678"
#endif

// AP 固定 IP 地址（四个字节分别定义，供 IPAddress 构造函数使用）
#ifndef AP_IP_1
#define AP_IP_1 192
#endif
#ifndef AP_IP_2
#define AP_IP_2 168
#endif
#ifndef AP_IP_3
#define AP_IP_3 44
#endif
#ifndef AP_IP_4
#define AP_IP_4 1
#endif

// -------------------------------------------------------------
// 显示缓冲区配置
// -------------------------------------------------------------
// LVGL 渲染缓冲区行数（总大小 = screenWidth * LV_BUFFER_LINES * 2 字节）
#ifndef LV_BUFFER_LINES
#define LV_BUFFER_LINES 30
#endif

// -------------------------------------------------------------
// UI 颜色配置（十六进制 RGB）
// -------------------------------------------------------------
// 充电中：蓝色
#ifndef UI_COLOR_CHARGING
#define UI_COLOR_CHARGING 0x318BD3
#endif
// 放电中：黄色
#ifndef UI_COLOR_DISCHARGING
#define UI_COLOR_DISCHARGING 0xFAD640
#endif
// 空闲背景：深灰
#ifndef UI_COLOR_IDLE_BG
#define UI_COLOR_IDLE_BG 0x262525
#endif
// 输入端口背景：绿色
#ifndef UI_COLOR_PORT_IN_BG
#define UI_COLOR_PORT_IN_BG 0x2CD16C
#endif
// 输出端口背景：红色
#ifndef UI_COLOR_PORT_OUT_BG
#define UI_COLOR_PORT_OUT_BG 0xCB3820
#endif
// 白色文字
#ifndef UI_COLOR_TEXT_WHITE
#define UI_COLOR_TEXT_WHITE 0xffffff
#endif

// -------------------------------------------------------------
// 电源管理配置
// -------------------------------------------------------------
// 功率阈值（W），低于此值持续 POWER_WINDOW_MS 毫秒时触发关机
#ifndef POWER_THRESHOLD
#define POWER_THRESHOLD 1.2f
#endif

// 低功率持续检测窗口（毫秒）
#ifndef POWER_WINDOW_MS
#define POWER_WINDOW_MS 60000UL
#endif

// 电池单体数量（用于能量计算）
#ifndef CELL_COUNT
#define CELL_COUNT 6
#endif

#endif // DISK_RAID_POWER_SETTINGS_H
