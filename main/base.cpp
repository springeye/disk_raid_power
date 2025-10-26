//
// Created by develop on 2025/9/29.
//

#include "base.h"

#include <cmath>
#include <esp32-hal-gpio.h>


#include <log.h>
#include <monitor_api.h>
// 全局变量：用于低功率持续检测（避免在 lambda 内被覆盖）
unsigned long g_powerCheckTimer = 0;
bool g_lowPowerWindowActive = false;
bool g_powerShutdownDone = false; // 若已触发关机，可用此标志避免重复触发
const float POWER_THRESHOLD = 0.7f;
const unsigned long POWER_WINDOW_MS = 30000UL;
void auto_power_off() {
    float bq_power = std::fabs(bq_get_power());
    mylog.printf("bq_power:%.2f\n", bq_power);
    // 使用全局状态变量，而非 lambda 局部静态变量，避免每次执行被覆盖
    if (fabs(bq_power) < POWER_THRESHOLD) // 如果功率持续低于阈值
    {
        if (!g_lowPowerWindowActive)
        {
            // 启动计时器
            g_powerCheckTimer = millis();
            g_lowPowerWindowActive = true;
            mylog.println("低功率窗口已激活");
        }
        else if (!g_powerShutdownDone && (millis() - g_powerCheckTimer >= POWER_WINDOW_MS))
        {
            // 持续低功率达到时间阈值，调用关机
            mylog.println("功率持续低于阈值达到设定时间，调用节能函数");
            digitalWrite(12, LOW); // 默认拉高（符合大多数硬件需求）
            g_powerShutdownDone = true; // 避免重复调用
        }
    }
    else
    {
        // 功率恢复时重置计时器（如果尚未触发关机）
        if (g_lowPowerWindowActive && !g_powerShutdownDone)
        {
            mylog.println("功率恢复，重置低功率窗口");
            g_lowPowerWindowActive = false;
            g_powerCheckTimer = 0;
        }
    }
    }
