# HAL KNOWLEDGE BASE

## OVERVIEW
硬件抽象层 (HAL) 目录，负责处理 ESP32、SDL2 桌面模拟和 STM32 三大平台的底层适配工作。

## STRUCTURE
```
hal/
├── esp32/            # ESP32 平台实现：TFT 初始化、LVGL 显示驱动、按键绑定
│   ├── app_hal.cpp   # ESP32 HAL 入口，包含 LGFX 初始化和 LVGL flush 回调
│   └── displays/     # 针对不同硬件板卡的显示配置文件 (*.hpp)
├── sdl2/             # 桌面模拟平台实现：创建 SDL 窗口，将鼠标映射为 LVGL 输入
└── stm32f429_disco/  # STM32F429 Discovery 开发板平台实现
```

## WHERE TO LOOK
| 任务 | 位置 | 说明 |
|------|------|------|
| 修改显示屏参数 | `hal/esp32/displays/` | 调整分辨率、背光引脚、SPI 频率 |
| 增加新硬件板卡 | `hal/esp32/app_hal.cpp` | include 新的 lgfx 配置并注册到 LGFX 实例 |
| 桌面模拟事件处理 | `hal/sdl2/app_hal.c` | 处理窗口缩放、鼠标交互、键盘映射 |
| 平台入口函数 | `hal/*/app_hal.h` | 定义通用的 `hal_setup()` 和 `hal_loop()` 接口 |

## CONVENTIONS
- **统一接口**：每个平台子目录必须导出 `hal_setup()` 和 `hal_loop()`，供 `main/main.cpp` 调用。
- **构建过滤**：通过 PlatformIO 的 `build_src_filter` 动态包含，编译时仅启用一个平台目录。
- **显示驱动 (ESP32)**：使用 LovyanGFX (LGFX) 作为底层驱动，在 `app_hal.cpp` 中通过 extern 声明 LGFX 实例。
- **输入映射**：ESP32 使用 LwBTN 处理按键，SDL2 直接将 SDL Event 转换为 LVGL `indev_data`。

## ANTI-PATTERNS
- ❌ **阻塞式 hal_loop**：绝对禁止在 `hal_loop` 中使用 `while(1)` 或长延时，必须保持非阻塞。
- ❌ **跨平台混写**：不要在 `hal/esp32` 中引入 SDL2 依赖，保持各平台目录代码独立。
- ⚠️ **手动切换配置**：当前 `app_hal.cpp` 仍通过手动注释来切换显示屏，添加新板卡时注意宏定义同步。
- ⚠️ **内存直接操作**：LVGL 缓冲区申请需遵循平台特性（如 ESP32-S3 优先使用 PSRAM）。
