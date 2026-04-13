## Context

本项目是一个 ESP32 嵌入式固件（磁盘/RAID 电源监控），使用 LVGL 9.3 + PlatformIO + Arduino 框架。当前代码由快速原型迭代而来，所有源码平铺在 `main/` 目录下，存在以下架构问题：

- **全局状态泛滥**：`IPortDevice* device`、`MyLog mylog`、驱动裸指针（bq/sw/ip2366）等全局对象散布各处
- **层级耦合严重**：monitor_api.cpp 同时承担设备数据读取和 LVGL UI 渲染，直接操作 UI 控件指针
- **无配置管理**：GPIO 引脚、I2C 地址、WiFi 凭证、阈值常量硬编码在源码中
- **无测试基础设施**：test/ 目录为空，无 CI 静态分析
- **命名不一致**：函数前缀混用 bq_get_*/bg_get_*/get2366*，风格混用 snake_case 和 camelCase

## Goals / Non-Goals

**Goals:**
- 建立清晰的分层架构：配置层 → 驱动层 → 设备抽象层 → 业务逻辑层 → UI 展示层
- 消除全局裸指针单例，改用依赖注入和显式生命周期管理
- 实现中央配置管理，消除硬编码常量
- 引入工程化工具链（代码格式化、静态分析、单元测试）
- 所有超长函数拆分为职责单一的小函数（<50 行）
- 修复已知 Bug（runSelfTest、extern "C" 拼写、注释不一致）

**Non-Goals:**
- 不引入新的外部依赖库（仅使用已有依赖）
- 不重写 UI 生成代码（ui.c/ui_schome.c 由 SquareLine Studio 生成，保持原样）
- 不改变硬件接线或引脚分配（仅将现有硬编码迁移到配置文件）
- 不实现 RTOS 多任务（保持当前单循环 + TaskScheduler 模型）
- 不修改第三方驱动代码（drivers/sw6306.c 仅修复编译错误）

## Decisions

### D1: 分阶段重构（Phase-based Refactoring）

**选择**：将重构分为 4 个阶段，每阶段独立编译通过
- Phase 1: 基础设施 + Bug 修复（无行为变更）
- Phase 2: 配置集中化 + 文件整理
- Phase 3: 架构解耦（依赖注入 + UI Presenter）
- Phase 4: 工程化完善（测试 + 静态分析 + 命名统一）

**理由**：嵌入式项目重构风险高，分阶段确保每步可验证、可回退。

**替代方案**：一次性大规模重构 — 风险过高，难以定位引入的问题。

### D2: 配置管理使用编译期宏 + PlatformIO build_flags

**选择**：创建 `settings.h` 集中所有硬件常量，通过 PlatformIO env 的 build_flags 覆盖。

**理由**：ESP32 嵌入式项目，运行时配置（NVS/LittleFS）会增加启动复杂度和 Flash 开销。编译期配置零开销，且 PlatformIO env 天然支持多硬件配置。

**替代方案**：运行时从 NVS/LittleFS 读取 — 增加启动时间和代码复杂度，对嵌入式固件不必要。

### D3: UI Presenter 模式解耦

**选择**：引入 `ui_presenter.h/cpp` 作为数据到 UI 的映射层。monitor_api 仅负责设备数据采集，返回 POD 结构体；ui_presenter 消费数据结构并更新 LVGL 控件。

**理由**：最小侵入式解耦 — 不修改 UI 生成代码，仅新增一个中间层。数据层完全不依赖 LVGL，可在 SDL2 模拟环境中单元测试。

**替代方案**：Observer/Event Bus 模式 — 过度设计，当前仅需单向数据推送。

### D4: 依赖注入使用显式初始化函数

**选择**：在 main.cpp 中显式构造所有硬件对象，通过 setter 函数注入到需要它们的模块。

**理由**：避免引入 DI 框架，保持嵌入式代码简洁。显式初始化顺序可控，便于调试。

**替代方案**：全局 unique_ptr + 延迟初始化 — 对 ESP32 而言增加了不必要的抽象层。

### D5: 错误处理使用 errno 风格 + 状态标志

**选择**：驱动层函数返回值保持原类型，新增 `hasError()` / `getLastError()` 查询接口。上层通过检查返回值 + 错误状态判断成功/失败。

**理由**：最小改动，兼容现有 C 接口。不引入 Result<T,E> 模板（增加代码体积），不改变函数签名。

**替代方案**：Result<T,E> 模板 — C/C++ 混合代码中模板使用困难，增加编译体积。

### D6: 测试框架使用 Unity（PlatformIO 原生）

**选择**：使用 PlatformIO 内置的 Unity 测试框架，在 SDL2 emulator 环境运行。

**理由**：零额外依赖，PlatformIO 原生支持，与 CI 集成简单。SDL2 环境可模拟硬件行为。

**替代方案**：Google Test / Catch2 — 对嵌入式项目过重，增加编译时间。

## Risks / Trade-offs

- **[重构范围大]** → 缓解：分 4 阶段实施，每阶段独立验证编译和功能
- **[UI 生成代码依赖]** → 缓解：不修改 ui.c/ui_schome.c，仅通过 ui_presenter 间接操作；若 SquareLine 重新生成，仅需确认控件名不变
- **[性能开销]** → 缓解：配置集中化为编译期宏零开销；UI Presenter 增加一次函数调用开销可忽略
- **[编译环境依赖]** → 缓解：LSP 报错来自 PlatformIO 依赖未在本地解析，实际编译由 PlatformIO 处理
- **[drivers/sw6306.c 第三方代码]** → 缓解：仅修复编译错误（extern "C" 拼写），不重构内部逻辑
- **[分阶段可能导致中间状态不一致]** → 缓解：每个 Phase 完成后确保全部 env 编译通过，并在实体硬件上验证基本功能

## Migration Plan

1. **Phase 1**（低风险，无行为变更）：添加 .clang-format、修复 runSelfTest bug、修复 extern "C" 拼写
2. **Phase 2**（中等风险，文件移动）：创建 settings.h 迁移硬编码常量、移动生成代码到 generated/、清理重复文件
3. **Phase 3**（高风险，架构变更）：拆分 monitor_api、引入 ui_presenter、实施依赖注入、拆分超长函数
4. **Phase 4**（中等风险，新增）：添加 Unity 测试、CI 静态分析步骤、统一命名

每个 Phase 完成后：编译所有 env（esp32_D0WDQ6、emulator_64bits）→ 通过 → 下一 Phase。

回退策略：每个 Phase 是独立 git commit，可直接 `git revert` 回退到上一个稳定状态。
