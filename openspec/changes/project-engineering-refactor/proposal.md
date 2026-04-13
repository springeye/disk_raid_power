## Why

当前项目由快速原型迭代而来，缺乏工程化基础设施：无测试、无静态分析、无统一配置管理、无清晰模块边界。代码中存在全局裸指针单例、UI 与硬件逻辑耦合、硬编码凭证、超长函数、命名不一致等问题，导致可维护性差、无法单元测试、新增硬件平台困难。项目已进入多版本迭代阶段，需要在架构层面进行系统性重构，以支持后续功能扩展和多人协作。

## What Changes

- **引入中央配置管理**：将所有硬编码常量（GPIO 引脚、I2C 地址、WiFi 凭证、IP 地址、缓冲大小、时间阈值）集中到配置头文件，支持 build-time 和 runtime 覆盖
- **解耦设备抽象与 UI 渲染**：将 monitor_api 拆分为纯数据层和 UI 展示层，消除设备逻辑对 LVGL 的直接依赖
- **引入依赖注入**：移除全局裸指针单例（device、bq、sw、ip2366 等），改为在 main 中显式构造并通过接口注入
- **统一错误处理策略**：引入错误码/Result 模式，使驱动层能区分"有效零值"与"读取失败"
- **拆分超长函数**：setup()、updateUI()、setup_ota()、getPortState()、read_word() 等拆分为职责单一的小函数
- **建立工程化基础设施**：添加 .clang-format、CI 静态分析步骤、单元测试框架（SDL2 模拟环境）
- **统一命名规范**：制定并执行一致的命名约定（public API camelCase，内部函数 snake_case）
- **修复已知 Bug**：runSelfTest() 返回值、SW6306.h extern "C" 拼写、scheduler 注释/参数不一致
- **整理文件结构**：分离生成代码（字体/图片/UI）到 generated/ 子目录，清理重复文件，规范驱动文件位置

## Capabilities

### New Capabilities
- `config-management`: 集中配置管理系统 — 硬件引脚、I2C 地址、WiFi 凭证、阈值常量的统一管理
- `device-abstraction`: 设备抽象与依赖注入 — IPortDevice 接口强化、工厂模式、生命周期管理
- `ui-presenter`: UI 展示层 — 数据模型到 LVGL 控件的映射层，解耦设备数据与 UI 渲染
- `error-handling`: 统一错误处理 — 驱动层错误码、Result 返回模式、上层错误传播
- `engineering-infra`: 工程化基础设施 — 代码格式化、静态分析、单元测试框架、CI 增强
- `module-restructure`: 模块化目录重构 — 文件分类整理、生成代码分离、驱动统一归置

### Modified Capabilities
<!-- 无已有 spec 需修改 -->

## Impact

- **核心文件变更**：main.cpp、monitor_api.cpp/h、KKPortDevice.cpp/h、ota.cpp、hal/esp32/app_hal.cpp、base.cpp 等几乎所有源文件
- **新增文件**：settings.h（配置）、ui_presenter.h/cpp（UI 展示层）、error_types.h（错误定义）、tests/（测试目录）
- **构建系统**：platformio.ini 需更新 build_flags 和 build_src_filter
- **CI**：.github/workflows/ci.yml 增加静态分析和测试步骤
- **依赖**：无新增外部依赖；内部依赖关系将大幅简化
- **风险**：涉及面广，需分阶段实施并确保每阶段编译通过；UI 生成代码（ui_schome.c）变更需谨慎
