## 1. Phase 1: Bug 修复与基础设施（低风险，无行为变更）

- [ ] 1.1 修复 main.cpp:35-41 runSelfTest() 返回值 — 注释说返回 true 但实际 return false，OTA 验证永远失败触发回滚
- [ ] 1.2 修复 main/drivers/sw6306.h:8 `extern C` 拼写错误为 `extern "C"`
- [ ] 1.3 修复 main.cpp:145 scheduler.addTask 注释"每2秒执行一次"但间隔参数为 1 的不一致
- [ ] 1.4 在项目根目录添加 .clang-format 配置文件（基于 LLVM 风格，缩进 4 空格，中文注释友好）
- [ ] 1.5 移除 main.cpp:25 `#define SPIFFS LittleFS` 宏别名，统一使用 LittleFS

## 2. Phase 2: 配置集中化与文件整理（中等风险）

- [ ] 2.1 创建 main/settings.h 配置头文件，集中定义：GPIO 引脚、I2C 地址/SCL/SDA、WiFi SSID/password、AP IP 地址、电芯数量、功率阈值、缓冲大小等常量，所有宏使用 `#ifndef` 保护以支持 build_flags 覆盖
- [ ] 2.2 更新 ota.cpp：将硬编码 ssid/password/IP 替换为 settings.h 中的宏引用
- [ ] 2.3 更新 KKPortDevice.cpp：将 wire1.begin(26,25)、SW6306(0x3C)、IP2366(14) 等硬编码替换为 settings.h 宏
- [ ] 2.4 更新 hal/esp32/app_hal.cpp：将 lvBufferSize 的 magic number 30 替换为 settings.h 中的可配置宏
- [ ] 2.5 更新 main.cpp：将 pinMode(12) 等硬编码引脚替换为 settings.h 宏，移除不必要的驱动头文件 include（bq40z80.h, ip2366.h, SW6306.h, i2c_utils.h）
- [ ] 2.6 创建 main/generated/ 目录，将 ui_font_*.c、ui_img_*.c 文件移动到该目录
- [ ] 2.7 更新 platformio.ini 的 build_src_filter 以包含 generated/ 子目录
- [ ] 2.8 删除重复的 main/ui_comp_hook.c（保留 main/components/ui_comp_hook.c）
- [ ] 2.9 移除或归档 main/drivers/1.47inch_Touch_LCD.ino（示例文件不应在生产代码中）
- [ ] 2.10 清理空目录（main/core/、main/devices/、main/services/、main/proto/、main/utils/）— 移除或添加 README.md 说明预留用途

## 3. Phase 3: 架构解耦（高风险，核心重构）

- [ ] 3.1 创建 main/data_types.h：定义 BatteryData、PortData、SystemData 等纯 POD 结构体，不依赖 LVGL
- [ ] 3.2 创建 main/ui_presenter.h/cpp：实现 ui_present_all() 和各子函数，接收 POD 数据结构并更新 LVGL 控件；仅 ui_presenter 包含 lvgl.h
- [ ] 3.3 重构 monitor_api.cpp：拆分 updateUI() 为 collectBatteryData() + collectPortData() + updateUI()（后者仅调用 ui_presenter），移除所有 LVGL 直接调用
- [ ] 3.4 在 monitor_api 中添加 monitor_api_set_device(IPortDevice*) 和 monitor_api_init() 函数
- [ ] 3.5 重构 KKPortDevice 类：将 BQ40Z80*、SW6306*、IP2366*、TwoWire 从文件作用域全局改为类成员变量
- [ ] 3.6 重构 main.cpp setup()：显式构造 KKPortDevice，调用 monitor_api_set_device() 注入设备，移除全局 new KKPortDevice()
- [ ] 3.7 拆分 main.cpp setup() 为子函数：init_serial()、init_storage()、init_hal()、init_ui()、init_device()、init_services()、register_tasks()
- [ ] 3.8 拆分 ota.cpp setup_ota() 为 init_wifi_ap()、setup_web_server()、setup_ota_routes()
- [ ] 3.9 拆分 KKPortDevice::getPortState() — 提取公共的端口状态判断逻辑为私有辅助方法 buildPortStatus()
- [ ] 3.10 从 monitor_api.h 中移除 extern IPortDevice* device 全局暴露，改为仅通过 setter/getter 访问
- [ ] 3.11 将 cell_helper.h 中对 lvgl.h 的依赖移除（cell_helper 不应依赖 UI 框架）

## 4. Phase 4: 工程化完善（中等风险）

- [ ] 4.1 为 BQ40Z80 类添加 hasError() 和 getLastError() 方法
- [ ] 4.2 为 IP2366 类添加 hasError() 和 getLastError() 方法
- [ ] 4.3 为 SW6306 类添加 hasError() 和 getLastError() 方法
- [ ] 4.4 更新驱动层：在 I2C 通信失败时设置错误状态并通过 mylog 记录设备名和操作类型
- [ ] 4.5 配置 PlatformIO Unity 测试框架，创建 test/test_cell_helper.cpp 基础测试
- [ ] 4.6 创建 test/test_utils.cpp 单元测试
- [ ] 4.7 更新 .github/workflows/ci.yml：添加 `pio test -e emulator_64bits` 测试步骤
- [ ] 4.8 统一 monitor_api.h 中 C API 函数命名：bq_get_* → battery_get_*, bg_get_* → battery_get_*, get2366* → port_c2_get_*
- [ ] 4.9 更新 main/utils.h：将 static 函数改为 inline 或移到 utils.cpp 中，避免多包含重复定义
- [ ] 4.10 运行 clang-format 格式化所有项目源文件（drivers/sw6306.c 第三方代码除外）
- [ ] 4.11 验证所有 PlatformIO env（esp32_D0WDQ6、esp32_D0WDQ6_201、emulator_64bits）编译通过
