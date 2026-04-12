# 源码目录知识库 (main/)

## OVERVIEW
本目录是 PlatformIO 的 `src_dir`，包含固件逻辑、设备驱动、LVGL UI 及非阻塞任务调度。

## STRUCTURE
- **核心入口**: `main.cpp` (setup/loop 及桌面 main)、`TaskScheduler.h` (非阻塞任务调度)
- **设备抽象层**: 
  - `PortDevices.h/cpp`: `IPortDevice` 抽象接口
  - `KKPortDevice.h/cpp`: 对应 `ESP32_169` 宏实现
- **桥接层**: `monitor_api.cpp` (C++ 设备对象到 LVGL C 回调的 `extern "C"` 桥接)
- **UI 层**: `ui.c/h` (LVGL 布局)、`ui_schome.c/h` (主页逻辑)、`lv_conf.h` (LVGL 配置)
- **硬件驱动**: 
  - 电池: `bq40z80.cpp`, `cell_helper.cpp` (电压能耗转换)
  - 充电: `ip2366.cpp`, `SW6306.cpp`
  - 通用: `key.cpp` (LwBTN), `temp.cpp` (温度), `i2c_utils.cpp`
- **系统服务**: `ota.cpp` (WiFi Web OTA), `log.cpp` (mylog)

## WHERE TO LOOK
| 任务 | 位置 |
|------|------|
| 修改主启动流程/任务频率 | `main.cpp` 中的 `setup()` 和 `scheduler.addTask` |
| LVGL C 代码调用 C++ 数据 | `monitor_api.cpp` 中的 `extern "C"` 包装函数 |
| 适配新硬件平台 | 实现 `IPortDevice` 并修改 `monitor_api.cpp` 的条件包含 |
| 调整电池电量计算曲线 | `cell_helper.cpp` 中的电压表 |
| UI 交互逻辑 (C 层) | `ui_schome.c` |

## CONVENTIONS
- **非阻塞循环**: `loop()` 仅调用 `scheduler.tick()`，严禁在任何地方使用 `delay()`
- **C 桥接**: 所有供 LVGL UI 调用的函数必须在 `monitor_api.cpp` 中以 `extern "C"` 导出
- **编译时分发**: 使用 `KKPortDevice` 作为唯一设备实现

## ANTI-PATTERNS
- ⚠️ **命名不规范**: 存在 `bg_get_*` 和 `bq_get_*` 混用情况 (如 `bg_get_temp`)
- ⚠️ **全局单例**: `device` 指针在 `monitor_api.cpp` 中全局定义，并在多处 extern 引用
- ⚠️ **单位不统一**: 电池电压在不同层级可能以 mV 或 V 为单位，调用前需确认
