# 项目规范化计划

> **原则**: 仅做结构/命名层面的改动，不改业务逻辑。使用 `git mv` 保持历史连续性。

---

## 一、目录结构重组

### 现状问题
- `main/` 下 20+ 源文件平铺，无逻辑分组
- 空占位目录：`main/core/`, `main/devices/`, `main/proto/`, `main/services/`, `main/utils/`, `fonts/`, `components/`
- 驱动和设备代码混在一起

### 目标结构

```
main/
├── main.cpp                  # 入口（不动）
├── app_hal.h                 # HAL 入口头文件（从 hal/esp32/ 统一引用）
├── lv_conf.h                 # LVGL 配置（不动）
├── settings.h                # 集中配置（不动）
├── data_types.h              # 数据类型定义（不动）
├── TaskScheduler.h           # 通用工具（不动）
│
├── drivers/                  # 硬件驱动层
│   ├── bq40z80.h/cpp         # 电池管理芯片驱动
│   ├── ip2366.h/cpp          # 充电芯片驱动
│   ├── sw6306_driver.h/cpp   # C++ 封装层（原 SW6306.h/cpp，重命名避免与第三方混淆）
│   ├── sw6306/               # 第三方 SW6306 寄存器级驱动（原 drivers/sw6306.c/h）
│   │   ├── sw6306.h
│   │   ├── sw6306.c
│   │   └── arduino_adapter.h/cpp  # I2C 适配层
│   ├── temp_sensor.h/cpp     # 温度传感器（原 temp.h/cpp）
│   └── key.h/cpp             # 按键驱动（不动）
│
├── device/                   # 设备抽象层（原 PortDevices + KKPortDevice）
│   ├── port_device.h         # IPortDevice 接口（原 PortDevices.h，重命名更语义化）
│   ├── port_device.cpp       # 空实现（原 PortDevices.cpp）
│   └── kk_port_device.h/cpp  # KK 硬件实现（原 KKPortDevice，snake_case 化）
│
├── monitor/                  # 监控/数据采集层
│   ├── monitor_api.h/cpp     # 监控 API（原 monitor_api）
│   ├── cell_helper.h/cpp     # 电芯显示辅助（不动）
│   ├── ui_presenter.h/cpp    # UI 数据绑定（不动）
│   └── power_manager.h/cpp   # 电源管理（原 base.h/cpp，语义化重命名）
│
├── services/                 # 系统服务层
│   └── ota.h/cpp             # OTA 升级服务（原 ota.h/cpp）
│
├── ui/                       # UI 生成代码（SquareLine 生成，不做格式化）
│   ├── ui.c/h
│   ├── ui_schome.c/h
│   ├── ui_helpers.c/h
│   ├── ui_events.h
│   └── ui_comp_hook.c        # 自定义 UI 组件
│
├── generated/                # 字体/图片资源（不动）
│   ├── ui_font_mibol10.c
│   ├── ...
│
├── log.h/cpp                 # 日志工具（保持在 main/ 根）
├── i2c_utils.h/cpp           # I2C 工具（保持在 main/ 根）
└── utils.h/cpp               # 通用工具（保持在 main/ 根）
```

### 操作清单
1. 删除空目录：`main/core/`, `main/devices/`, `main/proto/`, `main/utils/`, `fonts/`, `components/`
2. `git mv main/drivers/sw6306.c main/drivers/sw6306/sw6306.c` (创建子目录)
3. `git mv main/drivers/sw6306.h main/drivers/sw6306/sw6306.h`
4. `git mv main/drivers/arduino_adapter.* main/drivers/sw6306/`
5. `git mv main/SW6306.* main/drivers/sw6306_driver.*` (C++ 封装重命名)
6. `git mv main/bq40z80.* main/drivers/`
7. `git mv main/ip2366.* main/drivers/`
8. `git mv main/temp.* main/drivers/temp_sensor.*`
9. `git mv main/PortDevices.* main/device/port_device.*`
10. `git mv main/KKPortDevice.* main/device/kk_port_device.*`
11. `git mv main/monitor_api.* main/monitor/`
12. `git mv main/cell_helper.* main/monitor/`
13. `git mv main/ui_presenter.* main/monitor/`
14. `git mv main/base.* main/monitor/power_manager.*`
15. `git mv main/ota.* main/services/`
16. `git mv main/ui*.c main/ui/*.c` (UI 生成代码)
17. `git mv main/ui*.h main/ui/*.h`
18. `git mv main/components/ui_comp_hook.c main/ui/`
19. 删除 `main/filelist.txt`, `main/CMakeLists.txt`
20. 更新 `platformio.ini` 的 `build_src_filter` 匹配新路径

---

## 二、文件命名规范

### 规则：统一使用 `snake_case`

| 类型 | 规则 | 示例 |
|------|------|------|
| 源文件 | `snake_case` | `port_device.cpp` |
| 头文件 | `snake_case` | `port_device.h` |
| 目录名 | `snake_case` | `drivers/` |
| 类名 | `PascalCase` | `IPortDevice`, `KKPortDevice` |
| 函数名 | `snake_case` | `battery_get_percent()` |
| 变量名 | `snake_case` | `cell_voltage` |
| 宏/常量 | `UPPER_SNAKE_CASE` | `SW6306_ADDR` |
| 成员变量 | `snake_case_`（尾下划线） | `wire_`, `has_error_` |

### 需重命名的文件

| 原名 | 新名 | 原因 |
|------|------|------|
| `PortDevices.h/cpp` | `port_device.h/cpp` | PascalCase → snake_case |
| `KKPortDevice.h/cpp` | `kk_port_device.h/cpp` | CamelCase → snake_case |
| `SW6306.h/cpp` (main/) | `sw6306_driver.h/cpp` | ALL_CAPS → snake_case + 语义区分 |
| `temp.h/cpp` | `temp_sensor.h/cpp` | 语义更清晰 |
| `base.h/cpp` | `power_manager.h/cpp` | 语义更清晰 |
| `key.h/cpp` | 保持不变 | 已是 snake_case |
| `log.h/cpp` | 保持不变 | 已是 snake_case |

---

## 三、Include Guard 规范

### 规则：统一 `#ifndef` 格式

```c
#ifndef DISK_RAID_POWER_{NAME}_H
#define DISK_RAID_POWER_{NAME}_H
// ...
#endif // DISK_RAID_POWER_{NAME}_H
```

其中 `{NAME}` 使用大写文件名（不含路径前缀），例如 `port_device.h` → `DISK_RAID_POWER_PORT_DEVICE_H`。

### 修复清单

| 文件 | 当前 guard | 目标 guard |
|------|-----------|-----------|
| `bq40z80.h` | `__BQ40Z80_H` | `DISK_RAID_POWER_BQ40Z80_H` |
| `ip2366.h` | `IP2366_H` | `DISK_RAID_POWER_IP2366_H` |
| `sw6306_driver.h`（原 SW6306.h） | `_SW6306_H_` | `DISK_RAID_POWER_SW6306_DRIVER_H` |
| `i2c_utils.h` | `I2C_UTILS_H` | `DISK_RAID_POWER_I2C_UTILS_H` |
| `settings.h` | `#pragma once` | `DISK_RAID_POWER_SETTINGS_H` |
| `app_hal.h` | `APP_HAL_H` | `DISK_RAID_POWER_APP_HAL_H` |

**不动**：
- SquareLine 生成的文件（ui.h, ui_helpers.h, ui_events.h, ui_schome.h）保持原样
- 第三方库 `drivers/sw6306/sw6306.h` 保持原 guard `__SW6306_H__` 不改（仅移动位置，不改内容）

---

## 四、Include 路径规范

### 规则
- **系统/库头文件**：`#include <header.h>`（尖括号）
- **项目内头文件**：`#include "header.h"`（引号）

### 修复原则
所有 `main/` 内部引用改为引号形式：
```cpp
// 之前
#include <log.h>
#include <bq40z80.h>
#include <KKPortDevice.h>

// 之后
#include "log.h"
#include "drivers/bq40z80.h"
#include "device/kk_port_device.h"
```

**注意**：`platformio.ini` 的 `-I main` 确保了 `main/` 下的直接引用可行。重组后需更新 include 路径。

---

## 五、代码结构规范

### 5.1 头文件组织

每个 `.h` 文件统一结构：
```cpp
//
// {file_name}.{ext} — {简短描述}
//

#ifndef DISK_RAID_POWER_{GUARD}_H
#define DISK_RAID_POWER_{GUARD}_H

// 系统头文件
#include <stdint.h>

// 项目头文件
#include "port_device.h"

#ifdef __cplusplus
extern "C" {
#endif

// ... 声明 ...

#ifdef __cplusplus
}
#endif

#endif // DISK_RAID_POWER_{GUARD}_H
```

### 5.2 清理向后兼容宏

`monitor_api.h` 中 11 个兼容别名宏全部删除：
```cpp
// 删除这些
#define bq_get_percent battery_get_percent
#define bq_get_cell_voltage battery_get_cell_voltage
// ... 等共 11 个
```

这些是过渡期遗留，所有调用方已迁移到新名称。

### 5.3 清理空壳文件

- `PortDevices.cpp`（将变为 `device/port_device.cpp`）：当前为空（只有 include），确认是否需要保留
- `log.h`：移除空的 `extern "C" {}` 块

### 5.4 注释风格

统一使用中文注释，文件头格式：
```cpp
//
// {file_name}.{ext} — {中文简述}
//
```

---

## 六、构建配置更新

### platformio.ini 修改

```ini
# esp32 环境（不变）
[env:esp32]
build_src_filter =
  +<*>
  +<../hal/esp32>

# emulator_64bits 环境
[env:emulator_64bits]
build_src_filter =
  +<*>
  -<../hal/esp32>
  -<../hal/stm32f429_disco>
  +<../hal/sdl2>
  -<drivers/sw6306>           # 整个第三方 sw6306 子目录（含 arduino_adapter）
  -<drivers/sw6306_driver.cpp> # C++ 封装层（硬件依赖）
  -<device>                    # 整个 device 目录
  -<drivers/bq40z80.cpp>       # 电池驱动
  -<drivers/ip2366.cpp>        # 充电芯片驱动
  -<drivers/temp_sensor.cpp>   # 温度传感器
  -<services/ota.cpp>          # OTA 服务

# stm32f429_disco 环境（排除列表同 emulator_64bits）
[env:stm32f429_disco]
build_src_filter =
  +<*>
  -<../hal/esp32>
  -<../hal/sdl2>
  +<../hal/stm32f429_disco>
  -<drivers/sw6306>
  -<drivers/sw6306_driver.cpp>
  -<device>
  -<drivers/bq40z80.cpp>
  -<drivers/ip2366.cpp>
  -<drivers/temp_sensor.cpp>
  -<services/ota.cpp>
```

### build_flags `-I` 路径更新

需要为新的子目录添加 include 路径，或者改用相对路径引用。

---

## 七、执行顺序（分阶段，每阶段独立可编译）

### 阶段 1：纯重命名（零风险，编译验证）
1. 文件内容不变，仅 `git mv` 重命名文件
2. 更新所有 `#include` 路径
3. 更新 `platformio.ini` 的 `build_src_filter`
4. **验证**：
   - 命令：`pio run -e esp32_D0WDQ6`
   - 预期：`SUCCESS`，无编译错误

### 阶段 2：目录重组
1. 创建新目录结构
2. `git mv` 移动文件到目标目录
3. 更新所有 `#include` 路径（含目录前缀）
4. 更新 `platformio.ini` 的 `build_src_filter` 和 `-I` 路径
5. **验证**：
   - 命令：`pio run -e esp32_D0WDQ6`
   - 预期：`SUCCESS`

### 阶段 3：代码规范化
1. 统一 include guard 格式
2. 统一 include 引号风格
3. 删除向后兼容宏
4. 清理空壳代码
5. 统一文件头注释
6. **验证**：
   - 命令：`pio run -e esp32_D0WDQ6`
   - 预期：`SUCCESS`

### 阶段 4：清理与全环境验证
1. 删除空目录
2. 删除 `filelist.txt`, `CMakeLists.txt` 等无用文件
3. **验证**（全环境编译）：
   - `pio run -e esp32_D0WDQ6` → `SUCCESS`
   - `pio run -e esp32_D0WDQ6_201` → `SUCCESS`
   - `pio run -e emulator_64bits` → `SUCCESS`
   - `pio run -e stm32f429_disco` → `SUCCESS`
   - 可选：`pio run -e emulator_64bits -t execute` → 桌面模拟窗口正常显示

---

## 八、不动的文件（排除清单）

### 仅移动位置，内容不改
- `main/generated/*` — LVGL 生成资源
- `main/drivers/sw6306.c/h` — 第三方库（移动到 `drivers/sw6306/`，但 guard 和内容都不改）
- `main/lv_conf.h` — LVGL 配置文件

### 完全不动（不移动也不修改）
- `main/ui.c`, `main/ui.h` — SquareLine 生成
- `main/ui_schome.c/h` — SquareLine 生成
- `main/ui_helpers.c/h` — SquareLine 生成
- `main/ui_events.h` — SquareLine 生成
- `hal/*` — HAL 层已结构良好，不做改动
- `data/web/*` — Web UI 文件
- `.github/*` — CI 配置
- `main/TaskScheduler.h` — 通用工具头文件，保持原名不动（团队已有使用习惯）
