- 新建 `main/data_types.h` 时保持为纯数据头文件，仅依赖 `<cstdint>`，避免引入 LVGL、Arduino、PortDevices 造成循环依赖。
- `BatteryData`、`PortData`、`SystemData` 的字段应直接对齐 `monitor_api.cpp::updateUI()` 的实际数据来源，便于后续 UI/状态聚合。

## [Task 16] KKPortDevice 驱动成员化
- KKPortDevice 构造函数现在接收 TwoWire* 参数
- monitor_api.cpp 临时用 static TwoWire s_wire(1) 保持编译（Task 17 会彻底 DI）
- 析构函数 delete _bq/_sw/_ip2366，不 delete _wire（外部拥有）
## [Task 18] ui_presenter 层创建
- ui_present_all 参数为 const SystemData*（指针而非引用，兼容 C 调用）
- 不创建任何 LVGL 对象（无 lv_*_create 调用）
- 包含 lvgl.h + ui_schome.h + data_types.h + cell_helper.h
- update_cells() 仍在 presenter 末尾调用（来自 cell_helper）

## [Task 17] DI 完整注入
- monitor_api_set_device()/get_device() 是 C++ 函数（不在 extern "C" 块内）
- KKPortDevice 和 TwoWire 现在只在 main.cpp setup() 中构造
- static TwoWire s_wire(1) 声明为 static 保证对象不被栈销毁
- monitor_api.cpp 中 device 是 static IPortDevice* device = nullptr（不再自己构造）
- base.cpp 的 auto_power_off 通过 bq_get_power() extern "C" 访问设备，无需修改
- main.cpp 需新增 #include <Wire.h> 和 #include <KKPortDevice.h>（具体类型移至此处）
- monitor_api.cpp 删除 #include <KKPortDevice.h>（不再直接依赖具体实现）

## [Task 19] monitor_api 重构 — LVGL 解耦
- 移除 #include <lvgl.h>、#include "ui_schome.h"、#include <cell_helper.h>，改用 ui_presenter.h 和 data_types.h
- 新增两个私有静态辅助函数：collectBatteryData(BatteryData&) 和 collectPortData(PortData&, PortType)
- collectPortData 内部处理端口无活动时清零逻辑（voltage/current/power = 0.0f when !isCharging && !isDischarging）
- BatteryData.temp 为 int16_t，device->getBatTemp() 返回 float，需要 static_cast<int16_t> 转换
- updateUI() 精简至 ~20 行：device->loop() → collectBatteryData → collectPortData × 2 → boardTemp → totalInPower/totalOutPower → ui_present_all
- extern "C" 两个代码块中 13 个函数签名完全保留不变（函数体内容也不变）
- 所有 lv_label_*/lv_obj_set_* LVGL 调用全部移至 ui_presenter.cpp，monitor_api.cpp 零 LVGL 依赖
- LSP 报 PortType unknown 等错误均为环境问题（宏保护 #ifdef ESP32_169），PlatformIO 实际编译通过
- 验证命令：grep -c "lv_label\|lv_obj_set\|lvgl.h" main/monitor_api.cpp 返回 0

## [Task 20] setup() 拆分为子函数
- setup() 拆分为 8 个 static 子函数：init_serial、init_gpio、init_storage、init_hal、init_device、init_ui、init_services、register_tasks
- init_device() 被 init_ui() 内部调用（保持原始 DI 注入顺序：ui_init → init_cells → DI → updateUI → update_cells）
- static TwoWire s_wire(1) 保留在 init_device() 内部作为 local static，生命周期与程序相同（local static 保活）
- try-catch 块保留在 setup() 层面，包裹所有子函数调用
- 子函数均定义在 setup() 之前（无需 forward declaration）
- setup() 最终仅 9 行（含空行），远低于 30 行限制
- 桌面模拟 #ifndef ARDUINO 块的 LSP 报错（lv_init、show_gui 等）属于预期行为，原文件已存在，与本次改动无关
2026-04-13：重构 `KKPortDevice::getPortState()` 时，适合把 C1/C2 的“充电/放电/空闲”状态判断抽成统一的私有辅助方法，像 `buildPortStatus()` 这样只接收状态标志和电压/电流值，能明显压缩分支重复并让公共函数保持很短。
2026-04-13：`getPortState()` 里只负责采集各端口原始数据并转交给辅助方法，能减少后续维护时对两个端口逻辑的同步修改成本。

## [Task 21] setup_ota() 拆分为子函数
- 将 174 行的 setup_ota() 拆分为 3 个 static 子函数：init_wifi_ap()、setup_web_routes()、setup_ota_upload_route()
- setup_ota() 函数体压缩至 7 行（仅调用子函数 + server->begin()）
- OTA upload lambda（约 45 行）整体移入 setup_ota_upload_route()，lambda 内通过文件作用域变量访问全局状态，无需传参
- static 修饰确保子函数仅在本文件可见，符合封装原则
- LSP 报 WiFi.h / ESPAsyncWebServer.h 找不到属正常现象（ESP32 SDK 头文件在 PlatformIO 构建环境中，本地 LSP 无法解析），不影响实际编译
- pio run -e esp32_D0WDQ6 编译通过（SUCCESS）
# 2026-04-13
- 为 `BQ40Z80` 增加了 `hasError()` / `getLastError()` 状态查询接口，错误状态保存在类内而不是依赖 `state_flag` 外部推断。
- `read_word()` 在 I2C 发送失败、读取字节数异常、CRC 校验失败时分别记录 `_hasError` 与 `_lastError`，成功时清零，便于上层直接判断最近一次采样状态。
## 2026-04-13
- 头文件里仅声明、实现放到 `utils.cpp`，可以避免 `static` 函数在多个编译单元里产生重复实例，也更符合常规的接口/实现分离。
- 对于已经有 `.cpp` 文件的工具函数，优先把头文件中的 `static` 定义改成普通声明，保持函数签名不变，仅调整链接属性。
- 为 `IP2366` 增加 `hasError()` / `getLastError()` 时，优先在 `readRegister()` 这一层统一维护 `_hasError` / `_lastError`，让上层无需分别处理不同读寄存器路径的失败状态。

## 2026-04-13 - SW6306 错误状态
- 为 `SW6306` 增加 `hasError()` / `getLastError()`，用类内字段记录最近一次 I2C 失败，避免上层从返回值猜测总线状态。
- `readReg8()` / `readReg16()` 在 `endTransmission(false)` 失败时直接记录 Wire 错误码；在读取字节不足时记录 `-1`，成功读取后清空错误状态。
- 2026-04-13：clang-format 已在根目录创建，统一采用 LLVM 风格、4 空格缩进、120 列限制，适合本项目的中文注释与嵌入式代码风格。
