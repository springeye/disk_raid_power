# PROJECT KNOWLEDGE BASE

**Generated:** 2026-04-12
**Commit:** ed48676
**Branch:** master

## OVERVIEW
ESP32 嵌入式固件项目，用于磁盘/RAID 电源监控。LVGL 9.3 UI + PlatformIO + Arduino 框架，支持多硬件平台（ESP32-S3、ESP32-D0WDQ6、STM32、SDL2 桌面模拟）。

## STRUCTURE
```
disk_raid_power/
├── main/              # 源码（src_dir=main，非 src）—— 固件入口、设备驱动、UI、OTA
├── hal/               # 硬件抽象层（ESP32 / SDL2 / STM32 三平台实现）
├── data/web/          # OTA Web UI（HTML + JS + CSS）
├── support/           # 构建辅助脚本（SDL2 路径等）
├── test/              # 仅 README，无实际测试
├── fonts/             # 字体资源
├── components/        # 附加组件
├── platformio.ini     # 主构建配置（多 env 定义）
├── partition_150.csv  # Flash 分区表
└── .github/workflows/ # CI（跨平台 PlatformIO 构建）
```

## WHERE TO LOOK
| 任务 | 位置 | 说明 |
|------|------|------|
| 主入口/启动流程 | `main/main.cpp` | setup()/loop()（Arduino）+ main()（桌面模拟） |
| 设备抽象/端口驱动 | `main/monitor_api.cpp` | IPortDevice → KKPortDevice，extern "C" 桥接 LVGL |
| UI 布局 | `main/ui.c`, `main/ui_schome.c` | LVGL UI 生成代码 |
| 显示初始化/HAL | `hal/esp32/app_hal.cpp` | ESP32 平台：TFT 初始化、LVGL flush 回调 |
| OTA 升级 | `main/ota.cpp` | WiFi AP + HTTP OTA + AsyncWebServer |
| 电池管理 | `main/bq40z80.cpp` | bq40z80 芯片驱动 |
| 充电端口 | `main/ip2366.cpp`, `main/SW6306.cpp` | 充电协议芯片驱动 |
| 任务调度 | `main/TaskScheduler.h` | 非阻塞定时任务调度器 |
| LVGL 配置 | `main/lv_conf.h` | 16-bit 色、32K 内存、精简控件集 |
| 新增显示配置 | `hal/esp32/displays/` | 新建 `lgfx_{board}.hpp`，在 app_hal.cpp 中 include |

## CODE MAP
| 符号 | 类型 | 位置 | 角色 |
|------|------|------|------|
| `setup()` / `loop()` | 函数 | main/main.cpp | Arduino 固件主入口 |
| `hal_setup()` / `hal_loop()` | 函数 | hal/*/app_hal.c(pp) | 平台 HAL，编译时选择 |
| `IPortDevice` | 抽象类 | main/PortDevices.h | 端口设备接口 |
| `KKPortDevice` | 类 | main/KKPortDevice.cpp | ESP32_169 硬件实现 |
| `updateUI()` | 函数 | main/monitor_api.cpp | 刷新 UI 数据（C++ → C 桥接） |
| `runSelfTest()` | 函数 | main/main.cpp:35 | OTA 自检（⚠️ 注释/实现不一致） |
| `scheduler` | 实例 | main/main.cpp:76 | TaskScheduler 全局实例 |

## CONVENTIONS
- **src_dir = main**：PlatformIO 源码目录为 `main/`，非标准 `src/`
- **中文注释**：代码和注释全部使用中文
- **双模式编译**：`#ifdef ARDUINO` 区分固件/桌面模拟
- **设备选择**：编译宏 `ESP32_169` 选择 KKPortDevice 实现
- **extern "C" 桥接**：monitor_api.cpp 用 extern "C" 包装 C++ 设备方法，供 LVGL C 代码调用
- **非阻塞原则**：hal_loop() 和所有 scheduler 任务必须非阻塞
- **build_src_filter**：通过 PlatformIO build_src_filter 排除不适用的设备实现文件

## ANTI-PATTERNS（本项目）
- ⚠️ **`#define SPIFFS LittleFS`**：main.cpp:25，兼容旧代码的宏别名，掩盖 API 差异
- ⚠️ **runSelfTest() 注释说返回 true，实际 return false**：main.cpp:35-40，会导致 OTA 验证始终失败触发回滚
- ⚠️ **硬编码 WiFi 凭证**：ota.cpp 中 ssid/password 明文硬编码
- ⚠️ **显示配置手动注释**：app_hal.cpp 需手动注释/取消注释来切换显示驱动
- ❌ **hal_loop 中禁止 while 循环**：已注释 "NO while loop in this function!"

## COMMANDS
```bash
# 构建 ESP32-D0WDQ6 固件（默认 env）
pio run -e esp32_D0WDQ6

# 构建 + 上传
pio run -e esp32_D0WDQ6 -t upload

# 串口监控
pio device monitor -e esp32_D0WDQ6

# 桌面模拟（需 SDL2）
pio run -e emulator_64bits -t execute
```

## NOTES
- 空目录（devices/, core/, services/, proto/, utils/）为预留占位
- .pio/ 包含 PlatformIO 依赖缓存，体积大，已在 .gitignore
- 根目录存在异常文件 `nul`（Windows 保留名），可能干扰搜索工具
- PlatformIO env 继承链：env → esp32 → esp32_D0WDQ6
- OTA 自检逻辑（runSelfTest）当前有 bug，部署前需修复
