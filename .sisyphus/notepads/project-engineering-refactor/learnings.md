# Learnings - project-engineering-refactor

## 项目约定
- src_dir = main（非标准 src/）
- 中文注释
- #ifdef ARDUINO 区分固件/桌面
- build_src_filter 用于排除不适用文件
- 不可修改：ui.c, ui_schome.c, TaskScheduler.h
- 13 个 extern "C" 函数签名冻结（monitor_api.h）
- I2C 总线：bq40z80/SW6306/IP2366 共享单个 TwoWire
- 不引入 RTOS、事件总线、Observer 模式

## 构建命令
- pio run -e esp32_D0WDQ6
- pio run -e esp32_D0WDQ6_201
- pio run -e emulator_64bits -t execute
